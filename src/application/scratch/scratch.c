//==============================================================================================
/**
 * @file	scratch.c
 * @brief	「スクラッチ」メインソース
 * @author	Satoshi Nohara
 * @date	07.12.11
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
#include "system/touch_subwindow.h"
#include "system/msg_ds_icon.h"
#include "gflib/strbuf_family.h"
#include "savedata/config.h"
#include "application/p_status.h"
#include "poketool/pokeparty.h"
#include "poketool/poke_number.h"
#include "poketool/pokeicon.h"
#include "itemtool/itemsym.h"

#include "scratch_snd_def.h"
#include "scratch_sys.h"
#include "scratch_clact.h"
#include "scratch_bmp.h"
#include "scratch_obj.h"
#include "scratch.naix"
#include "application/scratch.h"

#include "msgdata/msg.naix"						//NARC_msg_??_dat
#include "msgdata/msg_scratch_contents.h"		//msg_??

#include "gflib/touchpanel.h"
#include "../../system/touchpanel_draw.h"


//==============================================================================================
//
//	デバック用
//
//==============================================================================================
//PROC_DATA* p_proc;


//==============================================================================================
//
//	定義
//
//==============================================================================================
//シーケンス定義
enum {
	SEQ_GAME_INIT,											//初期化

	SEQ_GAME_CARD_IN,										//カードイン
	SEQ_GAME_CARD_SEL,										//カード選択中
	SEQ_GAME_CARD_OUT,										//カードアウト

	SEQ_GAME_CARD_TOUCH_IN,									//カード削りイン
	SEQ_GAME_CARD_TOUCH,									//カード削り中
	SEQ_GAME_CARD_TOUCH_OUT,								//カード削りアウト

	SEQ_GAME_END,											//終了
};

//フォントカラー
#define	COL_BLUE	( GF_PRINTCOLOR_MAKE(FBMP_COL_BLUE,FBMP_COL_BLU_SDW,FBMP_COL_NULL) )//青
#define	COL_RED		( GF_PRINTCOLOR_MAKE(FBMP_COL_RED,FBMP_COL_RED_SDW,FBMP_COL_NULL) )	//赤

#define SCRATCH_END_WAIT		(10)				//終了するまでの表示ウェイト

#define SCRATCH_TP_NOT			(0xffff)			//タッチしていない時の定義

//ワクのパレット切り替え
//OS_Printf( "pal_no = %d\n", ScratchObj_PaletteNoGet( wk->p_d_waku[0] ) );
#define WAKU_PAL_END			(10)
#define WAKU_PAL_WAIT			(1)

//#define CENTER_ATARI_NO			(4)					//当たりが必ず配置される位置

#define SCRATCH_CARD_MODE_EASY	(0)		//イージカード

#define SCRATCH_CARD_POKE_MAX	(5)		//カードの中身のポケモンの種類の最大数

#define SCRATCH_EASY_ATARI_NUM	(4)		//カードに配置される当たりポケモンの数★
//#define SCRATCH_EASY_ATARI_NUM	(5)		//カードに配置される当たりポケモンの数★

#define SCRATCH_CARD_KEZURI_NUM	(3)		//カードを削れる数■
#define SCRATCH_CARD_ATARI_NUM	(3)		//当たりに必要な揃えるポケモンの数

#define SCRATCH_MOSAIC_MAX		(4)		//モザイクの最大幅


//==============================================================================================
//
//	OBJ配置データ
//
//	ポケ絵柄(5x5)
//
//==============================================================================================
//上：ポケモン配置位置
#define SCRATCH_U_POKE_X		(60)
#define SCRATCH_U_POKE_Y		(26)
#define SCRATCH_U_POKE_WIDTH_X	(41)
#define SCRATCH_U_POKE_WIDTH_Y	(42)

//下：当たりポケモン配置位置
static const SCRATCH_POS d_atari_pos[SCRATCH_CARD_ATARI_NUM] = {
	{ 52, 68 },	{ 108, 68 }, { 164, 68 }, 
};

//下：当たりウィンドウ配置位置
static const SCRATCH_POS d_atari_win_pos = { 38, 56 };

//下：ポケモン配置位置
static const SCRATCH_POS d_poke_pos[SCRATCH_D_POKE_MAX] = {
#if 0
	{ 52, 18 },	{ 124, 18 },	{ 196, 18 }, 
	{ 52, 66 },	{ 124, 66 },	{ 196, 66 }, 
	{ 52, 114 },{ 124, 114 },	{ 196, 114 }, 
#else
	//+5						//-5
	{ 57, 18 },	{ 124, 18 },	{ 191, 18 }, 
	{ 57, 66 },	{ 124, 66 },	{ 191, 66 }, 
	{ 57, 114 },{ 124, 114 },	{ 191, 114 }, 
#endif
};

//銀箔のワクの表示オフセット(下：ポケモン配置に足しこむ)
//#define SCRATCH_GIN_WAKU_OFFSET_X	(-20)
#define SCRATCH_GIN_WAKU_OFFSET_X	(-36)
#define SCRATCH_GIN_WAKU_OFFSET_Y	(-10)

//下：カード位置
static const SCRATCH_POS d_card_pos[SCRATCH_D_CARD_MAX] = {
	{ 0, 36 },	{ 56, 36 }, { 112, 36 },	{ 168, 36 },
};

//下：ボタン配置位置
static const SCRATCH_POS d_next_button_pos = { 68, 160 };
static const SCRATCH_POS d_yameru_button_pos = { 88, 160 };

//下：真ん中ウィンドウ配置位置
//static const SCRATCH_POS d_win_pos = { 68, 64 };
static const SCRATCH_POS d_win_pos = { 68, 68 };

//拡大する時の目標位置
//static const SCRATCH_POS card_center_pos = { 4, 8 };
//static const SCRATCH_POS card_center_pos = { 16, 4 };
//static const SCRATCH_POS card_center_pos = { 48, 4 };
static const SCRATCH_POS card_center_pos = { 64, 4 };

//#define SCRATCH_CARD_IN_OFFSET_X	(-132)			//カードがインする時の表示オフセット
#define SCRATCH_CARD_IN_OFFSET_X	(-256)			//カードがインする時の表示オフセット
//#define SCRATCH_CARD_IN_SPD		(4)				//カードがインするスピード
//#define SCRATCH_CARD_IN_SPD			(8)				//カードがインするスピード
#define SCRATCH_CARD_IN_SPD			(32)				//カードがインするスピード
//#define SCRATCH_CARD_IN_SPD			(32)				//カードがインするスピード
#define SCRATCH_CARD_IN_SPD2		(16)				//カードがインするスピード

//#define SCRATCH_CARD_OUT_SPD		(4)				//カードがアウトするスピード
//#define SCRATCH_CARD_OUT_SPD		(8)				//カードがアウトするスピード
#define SCRATCH_CARD_OUT_SPD		(16)				//カードがアウトするスピード
//#define SCRATCH_CARD_OUT_OFFSET_X	(200)			//カードがアウトする時の表示オフセット
#define SCRATCH_CARD_OUT_OFFSET_X	(256)			//カードがアウトする時の表示オフセット

#define SCRATCH_CARD_AFF_WAIT		(1)				//カードが拡大するウェイト
//#define SCRATCH_CARD_AFF_MAX		(0x2000)		//カードの最大拡大
//#define SCRATCH_CARD_AFF_MAX		(0x1e00)		//カードの最大拡大
//#define SCRATCH_CARD_AFF_MAX		(0x1b00)		//カードの最大拡大
//#define SCRATCH_CARD_AFF_MAX		(0x1a00)		//カードの最大拡大
//#define SCRATCH_CARD_AFF_MAX		(0x1900)		//カードの最大拡大
#define SCRATCH_CARD_AFF_MAX		(0x1800)		//カードの最大拡大
#define SCRATCH_CARD_AFF_ADD		(0x0100)		//カードの拡大
#define SCRATCH_CARD_AFF_DEFAULT	(0x1000)		//カードの初期倍率(等倍)

//削り
//#define SCRATCH_CARD_TOUCH_IN_OFFSET_X	(-256)	//削りカードがインする時の表示オフセット
#define SCRATCH_CARD_TOUCH_IN_OFFSET_X	(256)		//削りカードがインする時の表示オフセット
//#define SCRATCH_CARD_TOUCH_IN_SPD		(-8)		//削りカードがインするスピード
#define SCRATCH_CARD_TOUCH_IN_SPD		(-16)		//削りカードがインするスピード
//#define SCRATCH_CARD_TOUCH_OUT_SPD		(-8)		//削りカードがアウトするスピード
#define SCRATCH_CARD_TOUCH_OUT_SPD		(-16)		//削りカードがアウトするスピード
//#define SCRATCH_CARD_TOUCH_OUT_SPD		(8)		//削りカードがアウトするスピード
#define SCRATCH_CARD_TOUCH_OUT_OFFSET_X	(-256)		//カードがアウトする時の表示オフセット

//カードの色ナンバー
enum{
	CARD_BLUE = 0,		//左上
	CARD_RED,			//右上
	CARD_YELLOW,		//左下
	CARD_GREEN,			//右下
};

//#define CARD_CENTER_SPD_X		(8)		//カードを中心にする速度
//#define CARD_CENTER_SPD_Y		(8)
//#define CARD_CENTER_SPD_X		(16)	//カードを中心にする速度
//#define CARD_CENTER_SPD_Y		(16)
//#define CARD_CENTER_SPD_X		(16)	//カードを中心にする速度
//#define CARD_CENTER_SPD_Y		(12)
#define CARD_CENTER_SPD_X		(24)	//カードを中心にする速度
#define CARD_CENTER_SPD_Y		(16)

//ダミーナンバー
enum{
	DUMMY_CARD_POKE = 0xb0,
	DUMMY_KEZURI_CARD1,
	DUMMY_KEZURI_CARD2,
	DUMMY_KEZURI_CARD3,
};


//==============================================================================================
//
//	貰える木の実
//
//==============================================================================================
//イージーカード
static const u16 scratch_easy_item[] = {
	ITEM_ZAROKUNOMI,
	ITEM_NEKOBUNOMI,
	ITEM_TAPORUNOMI,
	ITEM_ROMENOMI,
	ITEM_UBUNOMI,
	ITEM_MATOMANOMI,
	////////////////
	ITEM_OKKANOMI,
	ITEM_ITOKENOMI,
	ITEM_SOKUNONOMI,
	ITEM_RINDONOMI,
	ITEM_YATHENOMI,
	ITEM_YOPUNOMI,
	ITEM_BIAANOMI,
	ITEM_SYUKANOMI,
	ITEM_BAKOUNOMI,
	ITEM_UTANNOMI,
	ITEM_TANGANOMI,
	ITEM_YOROGINOMI,
	ITEM_KASIBUNOMI,
	ITEM_HABANNOMI,
	ITEM_NAMONOMI,
	ITEM_RIRIBANOMI,
	ITEM_HOZUNOMI,
};
#define SCRATCH_EASY_ITEM	( NELEMS(scratch_easy_item) )


//==============================================================================================
//
//	カードのタッチ判定
//
//==============================================================================================
static const RECT_HIT_TBL card_hit_tbl[SCRATCH_D_CARD_MAX+1] = {
	//top,bottom,left,right
	{ 42,	154,	0,		80 },			//一番左
	{ 42,	154,	88,		138 },			//
	{ 42,	154,	144,	195 },			//
	{ 42,	154,	204,	254 },			//一番右
	{ TP_HIT_END,0,0,0 },					//終了データ
};

#define SCRATCH_START_MSG_DEL_WAIT		( 30 )			//スクラッチスタートを消すまでのウェイト
#define SCRATCH_RESULT_MSG_DEL_WAIT		( 60 )			//スクラッチ結果を消すまでのウェイト
#define SCRATCH_GIN_ALL_CLEAR_WAIT		( 30 )			//銀箔を全て消した状態のウェイト
#define SCRATCH_RESULT_MOSAIC_WAIT		( 30 )			//メタモンのモザイクが始まるまでのウェイト


//==============================================================================================
//
//	つぎのカードへ・やめるボタンのタッチ判定
//
//==============================================================================================
static const RECT_HIT_TBL next_hit_tbl[] = {
	{ 164,	191,	76,		180 },			//
	{ TP_HIT_END,0,0,0 },					//終了データ
};

static const RECT_HIT_TBL yameru_hit_tbl[] = {
	{ 164,	191,	94,		164 },			//
	{ TP_HIT_END,0,0,0 },					//終了データ
};


//==============================================================================================
//
//	銀箔ブロックごとのタッチ判定(銀箔のサイズ分)
//
//==============================================================================================
static const RECT_HIT_TBL block_hit_tbl[] = {	//★注意！block_hit_tbl2と同じ値にする
	//top,bottom,left,right
#if 0
	{ 21,	54,		27,		87 },			//1段目
	{ 21,	54,		102,	162 },			//
	{ 21,	54,		174,	232 },			//

	{ 68,	100,	27,		87 },			//2段目
	{ 68,	100,	102,	162 },			//
	{ 68,	100,	174,	232 },			//

	{ 114,	148,	27,		87 },			//3段目
	{ 114,	148,	102,	162 },			//
	{ 114,	148,	174,	232 },			//
#else
	{ 21,	54,		32,		89 },			//1段目			//+5
	{ 21,	54,		102,	157 },			//
	{ 21,	54,		170,	227 },			//				//-5

	{ 68,	100,	32,		89 },			//2段目
	{ 68,	100,	102,	157 },			//
	{ 68,	100,	170,	227 },			//

	{ 114,	148,	32,		89 },			//3段目
	{ 114,	148,	102,	157 },			//
	{ 114,	148,	170,	227 },			//
#endif
	{ TP_HIT_END,0,0,0 },					//終了データ
};

#define SCRATCH_TOUCH_SIZE_X	(240)
#define SCRATCH_TOUCH_SIZE_Y	(160)
#define SCRATCH_TOUCH_SIZE_MAX	(SCRATCH_TOUCH_SIZE_X * SCRATCH_TOUCH_SIZE_Y)

//RECT構造体(RECT_HIT_TBLの値を直接参照出来ないので同じデータを複数用意)
typedef struct{
	u8 top;
	u8 bottom;
	u8 left;
	u8 right;
}RECT;
static const RECT block_hit_tbl2[] = {		//★注意！block_hit_tblと同じ値にする
	//top,bottom,left,right
#if 0
	{ 21,	54,		27,		87 },			//1段目
	{ 21,	54,		102,	162 },			//
	{ 21,	54,		174,	232 },			//

	{ 68,	100,	27,		87 },			//2段目
	{ 68,	100,	102,	162 },			//
	{ 68,	100,	174,	232 },			//

	{ 114,	148,	27,		87 },			//3段目
	{ 114,	148,	102,	162 },			//
	{ 114,	148,	174,	232 },			//
#else
	{ 21,	54,		32,		89 },			//1段目			//+5
	{ 21,	54,		102,	157 },			//
	{ 21,	54,		170,	227 },			//				//-5

	{ 68,	100,	32,		89 },			//2段目
	{ 68,	100,	102,	157 },			//
	{ 68,	100,	170,	227 },			//

	{ 114,	148,	32,		89 },			//3段目
	{ 114,	148,	102,	157 },			//
	{ 114,	148,	170,	227 },			//
#endif
};


//==============================================================================================
//
//	銀箔の削り率をチェックする座標テーブル(この座標からサイズ分をチェックする)
//
//==============================================================================================
static const SCRATCH_POS block_check_pos[SCRATCH_D_POKE_MAX] = {
#if 0
	{ 46,	27	 },			//1段目
	{ 121,	27	 },			//
	{ 193,	27	 },			//

	{ 46,	74	 },			//2段目
	{ 121,	74	 },			//
	{ 193,	74	 },			//

	{ 46,	120  },			//3段目
	{ 121,	120  },			//
	{ 193,	120  },			//
#else
	{ 42,	27	 },			//1段目
	{ 117,	27	 },			//
	{ 189,	27	 },			//

	{ 42,	74	 },			//2段目
	{ 117,	74	 },			//
	{ 189,	74	 },			//

	{ 42,	120  },			//3段目
	{ 117,	120  },			//
	{ 189,	120  },			//
#endif
};
#define BLOCK_SIZE_X	(25)
#define BLOCK_SIZE_Y	(20)				//X * Y = 500
//#define BLOCK_COUNT	(35)				//この値までカウントされたら全て公開
//#define BLOCK_COUNT	(70)				//この値までカウントされたら全て公開
#define BLOCK_COUNT		(380)				//この値までカウントされたら全て公開


//==============================================================================================
//
//	メッセージ関連の定義
//
//==============================================================================================
#define SCRATCH_MENU_BUF_MAX		(2)						//メニューバッファの最大数
#define BS_FONT					(FONT_SYSTEM)			//フォント種類
#define SCRATCH_MSG_BUF_SIZE		(600)//(800)//(1024)			//会話のメッセージsize
#define SCRATCH_MENU_BUF_SIZE		(32)					//メニューのメッセージsize
#define PLAYER_NAME_BUF_SIZE	(PERSON_NAME_SIZE + EOM_SIZE)	//プレイヤー名のメッセージsize
#define POKE_NAME_BUF_SIZE		(MONS_NAME_SIZE + EOM_SIZE)		//ポケモン名のメッセージsize


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
struct _SCRATCH_WORK{

	PROC* proc;										//PROCへのポインタ
	PROC* child_proc;								//PROCへのポインタ

	u8	sub_seq;									//シーケンス
	u8	type;										//引数として渡されたバトルタイプ
	u8	msg_index;									//メッセージindex
	u8	tmp_csr_pos;								//退避してあるカーソル位置

	u8	csr_pos;									//現在のカーソル位置
	u8	wait;
	u8	card_num;									//選択したカード枚数
	u8	time_wait;

	s16	counter;
	s16	counter2;

	u8	mosaic_size;								//モザイクサイズ(0-15)

	u8	card_col[SCRATCH_D_CARD_MAX];				//選んだカードの場所(色ナンバーを格納)
	u8	mode[SCRATCH_CARD_NUM];

	VecFx32 scale_fx;

	VecFx32 scale_tbl;

	void*	msgicon;
	MSGDATA_MANAGER* msgman;						//メッセージマネージャー
	WORDSET* wordset;								//単語セット
	STRBUF* msg_buf;								//メッセージバッファポインタ
	STRBUF* tmp_buf;								//テンポラリバッファポインタ

	STRCODE str[PERSON_NAME_SIZE + EOM_SIZE];		//メニューのメッセージ

	GF_BGL_INI*	bgl;								//BGLへのポインタ
	GF_BGL_BMPWIN bmpwin[SCRATCH_BMPWIN_MAX];			//BMPウィンドウデータ

	PALETTE_FADE_PTR pfd;							//パレットフェード

	NUMFONT* num_font;								//8x8フォント

	//const CONFIG* config;							//コンフィグポインタ
	CONFIG* config;									//コンフィグポインタ
	SAVEDATA* sv;									//セーブデータポインタ

	SCRATCH_CLACT scratch_clact;					//セルアクタデータ
	SCRATCH_OBJ* p_u_poke[SCRATCH_U_POKE_MAX];		//ポケモンOBJのポインタ格納テーブル
	SCRATCH_OBJ* p_d_poke[SCRATCH_D_POKE_MAX];		//ポケモンOBJのポインタ格納テーブル
	SCRATCH_OBJ* p_d_card[SCRATCH_D_CARD_MAX];		//カードOBJのポインタ格納テーブル

	SCRATCH_OBJ* p_d_button;						//下画面ボタン
	SCRATCH_OBJ* p_d_win;							//下画面の真ん中のウィンドウ
	SCRATCH_OBJ* p_d_waku[SCRATCH_CARD_ATARI_NUM];	//下画面の銀箔のワクOBJのポインタ格納テーブル
	SCRATCH_OBJ* p_d_atari[SCRATCH_CARD_ATARI_NUM];	//当たりポケモンOBJのポインタ格納テーブル
	SCRATCH_OBJ* p_d_atari_win;						//当たりウィンドウの真ん中のウィンドウ

	u8 card_poke[SCRATCH_D_POKE_MAX];				//カードの中にいるポケモン

	u16 atari_item[SCRATCH_ATARI_MAX];				//当たりの商品

	u16* p_ret_work;								//SCRATCH_CALL_WORKの戻り値ワークへのポインタ
	u16* p_item_no;									//SCRATCH_CALL_WORKのitemナンバ－へのポインタ
	u16* p_item_num;								//SCRATCH_CALL_WORKのitem個数へのポインタ

	ARCHANDLE* hdl;

	TP_BRUSH_DATA brush;							//ブラシデータ

	//キャラクタデータ
	void* p_char;
	NNSG2dCharacterData* p_chardata;
	u8* buf;

	TOUCH_SW_SYS* touch_subwin;

	u8 block_flag[SCRATCH_D_POKE_MAX];				//どのブロックの銀箔に触れたかフラグ

	u8 pltt_res_no;
	u8 pltt_wait;
	u8 atari_poke;
	u8 waku_flag:1;									//リーチフラグ=1
	u8 move_num:7;									//カード中心への移動回数

	u8 kezuri_card_num;								//削り率OKなカードの数
	u8 kezuri_card[SCRATCH_CARD_KEZURI_NUM];		//削ったカード

	u8 block_count[SCRATCH_D_POKE_MAX];

	u32 tp_x;
	u32 tp_y;
	TP_ONE_DATA	tpData;

	//u8 touch_flag[256*192/8];
	//u8 touch_flag[256*192];
	u8 touch_flag[SCRATCH_TOUCH_SIZE_MAX];
};


//==============================================================================================
//
//	タッチパネル
//
//==============================================================================================
//キャラ数
#define NUM_X_MAX			(32)
#define NUM_Y_MAX			(24)
#define NUM_MAX				(NUM_X_MAX * NUM_Y_MAX)
//ドット数
#define DOT_X_MAX			(NUM_X_MAX * 8)
#define DOT_Y_MAX			(NUM_Y_MAX * 8)
#define DOT_MAX				(DOT_X_MAX * DOT_Y_MAX)

//16色の1キャラのサイズ
#define ONE_CHAR			(0x20)

//触れた位置からクリアするdot(上下、左右で倍になる)
//#define DOT_WRITE_WIDTH		(2)
#define DOT_WRITE_WIDTH		(3)


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
//PROC
PROC_RESULT ScratchProc_Init( PROC * proc, int * seq );
PROC_RESULT ScratchProc_Main( PROC * proc, int * seq );
PROC_RESULT ScratchProc_End( PROC * proc, int * seq );

//シーケンス
static BOOL Seq_GameInit( SCRATCH_WORK* wk );
static BOOL Seq_GameCardIn( SCRATCH_WORK* wk );
static BOOL Seq_GameCardSel( SCRATCH_WORK* wk );
static BOOL Seq_GameCardOut( SCRATCH_WORK* wk );
static BOOL Seq_GameCardTouchIn( SCRATCH_WORK* wk );
static BOOL Seq_GameCardTouch( SCRATCH_WORK* wk );
static BOOL Seq_GameCardTouchOut( SCRATCH_WORK* wk );
static BOOL Seq_GameEnd( SCRATCH_WORK* wk );

//共通処理
static void Scratch_WakuAdd( SCRATCH_WORK* wk );
static void Scratch_AtariAdd( SCRATCH_WORK* wk );
static void Scratch_ButtonAdd( SCRATCH_WORK* wk );
static void Scratch_CardDelete( SCRATCH_WORK* wk );
static void ScratchCommon_Delete( SCRATCH_WORK* wk );
static void Scratch_Recover( SCRATCH_WORK* wk );
static void Scratch_InitSub1( void );
static void Scratch_InitSub2( SCRATCH_WORK* wk );
static void Scratch_AllocTouchSub( SCRATCH_WORK* wk );
static void Scratch_InitTouchSub( SCRATCH_WORK* wk );
static void Scratch_FreeTouchSub( SCRATCH_WORK* wk );

//共通初期化、終了
static void Scratch_ObjInit( SCRATCH_WORK* wk );
static void Scratch_BgInit( SCRATCH_WORK* wk );
static void Scratch_BgExit( GF_BGL_INI * ini );

//設定
static void VBlankFunc( void * work );
static void SetVramBank(void);
static void SetBgHeader( GF_BGL_INI * ini );

//BGグラフィックデータ
static void Scratch_SetMainBgGraphic( SCRATCH_WORK * wk, u32 frm  );
static void Scratch_SetMainBgPalette( void );
static void Scratch_SetYakuBgGraphic( SCRATCH_WORK * wk, u32 frm  );
static void Scratch_SetBackBgGraphic( SCRATCH_WORK * wk, u32 frm  );
static void Scratch_SetSubBgGraphic( SCRATCH_WORK * wk, u32 frm  );
static void Scratch_SetSubBgPalette( void );
static void Scratch_SetTouchBgGraphic( SCRATCH_WORK * wk, u32 frm  );
//static void Scratch_SetAtariBgGraphic( SCRATCH_WORK * wk, u32 frm  );

//メッセージ
static u8 ScratchWriteMsg( SCRATCH_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font );
static u8 ScratchWriteMsg_ov111_21D2424( SCRATCH_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font );
static u8 ScratchWriteMsgSimple( SCRATCH_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font );
static u8 Scratch_KakuninMsg( SCRATCH_WORK* wk );
//static u8 Scratch_CardNumMsg( SCRATCH_WORK* wk );
static u8 Scratch_CardSelectMsg( SCRATCH_WORK* wk );
static u8 Scratch_StartMsg( SCRATCH_WORK* wk );
static u8 Scratch_AtariMsg( SCRATCH_WORK* wk );
static u8 Scratch_HazureMsg( SCRATCH_WORK* wk );
static u8 Scratch_NextMsg( SCRATCH_WORK* wk );
static u8 Scratch_YameruMsg( SCRATCH_WORK* wk );
static u8 Scratch_YakuMsg( SCRATCH_WORK* wk, u8 no );

//文字列
static void Scratch_SetNumber( SCRATCH_WORK* wk, u32 bufID, s32 number );
static void Scratch_SetPokeName( SCRATCH_WORK* wk, u32 bufID, POKEMON_PASO_PARAM* ppp );
static void Scratch_SetPlayerName( SCRATCH_WORK* wk, u32 bufID );
static void PlayerNameWrite( SCRATCH_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font );
static void PokeNameWriteEx( SCRATCH_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font, u16 monsno, u8 sex );

//ツール
static void NextSeq( SCRATCH_WORK* wk, int* seq, int next );
static int KeyCheck( int key );
static void BgCheck( SCRATCH_WORK* wk );
static u16 GetCsrX( SCRATCH_WORK* wk );
static void Scratch_MetamonPokeSet( SCRATCH_WORK* wk );
static void Scratch_OthersPokeSet( SCRATCH_WORK* wk );
static void Scratch_PokeSet( SCRATCH_WORK* wk );
static void Scratch_CardPokeSet( SCRATCH_WORK* wk, u8 mode );
static void Scratch_CardPokeClear( SCRATCH_WORK* wk );
static void Scratch_YakuSet( SCRATCH_WORK* wk, u8 mode );
static BOOL Scratch_CardCenterMove( SCRATCH_WORK* wk, u8 pos );
static void ScrPalChg( SCRATCH_WORK* wk, u32 frm, u8 pltt_no, u8 sx, u8 sy );
static u8 Scratch_BlockFlagOnNumGet( SCRATCH_WORK* wk );
static void Scratch_BlockFlagClear( SCRATCH_WORK* wk );
static void Scratch_BlockCountClear( SCRATCH_WORK* wk );
static void Scratch_HoseiTouch2( SCRATCH_WORK* wk, int index );
static void Scratch_TouchFlagSet( SCRATCH_WORK* wk );
static void Scratch_TouchFlagSetSub( SCRATCH_WORK* wk, int tp_x, int tp_y );
static BOOL Scratch_TouchFlagCheck( SCRATCH_WORK* wk, u8 index );
static void Scratch_PlttNoResChg( SCRATCH_WORK* wk );
static BOOL Scratch_AtariCheck( SCRATCH_WORK* wk );
static BOOL Scratch_MosaicEffInit( SCRATCH_WORK* wk );
static BOOL Scratch_MosaicEffMain( SCRATCH_WORK* wk, u8 flag );

//サブシーケンス
static void Scratch_SeqSubYameruCard( SCRATCH_WORK* wk );
static void Scratch_SeqSubNextCard( SCRATCH_WORK* wk );
static void Scratch_SeqSubGinOk( SCRATCH_WORK* wk );

//キャラクターデータをバッファにコピー
static void Scratch_Touch_0( SCRATCH_WORK* wk );
//バッファを生成
static void Scratch_Touch_1( SCRATCH_WORK* wk );
//キャラクターデータをコピー
static void Scratch_Touch_1_5( SCRATCH_WORK* wk );
//タッチパネルに触れている座標を取得してキャラクターデータを書き換える
static void Scratch_Touch_2( SCRATCH_WORK* wk, int tp_x, int tp_y );
//渡された位置のキャラクターデータを0クリア
static void Scratch_Touch_3( SCRATCH_WORK* wk, u32 tp_x, u32 tp_y );


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
PROC_RESULT ScratchProc_Init( PROC * proc, int * seq )
{
	int i;
	SCRATCH_WORK* wk;
	SCRATCH_CALL_WORK* scratch_call;

	Scratch_InitSub1();

	//sys_CreateHeap( HEAPID_BASE_APP, HEAPID_SCRATCH, 0x20000 );
	//sys_CreateHeap( HEAPID_BASE_APP, HEAPID_SCRATCH, 0x30000 );
	//sys_CreateHeap( HEAPID_BASE_APP, HEAPID_SCRATCH, 0x38000 );
	//sys_CreateHeap( HEAPID_BASE_APP, HEAPID_SCRATCH, 0x40000 );
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_SCRATCH, 0x48000 );

	wk = PROC_AllocWork( proc, sizeof(SCRATCH_WORK), HEAPID_SCRATCH );
	memset( wk, 0, sizeof(SCRATCH_WORK) );

	wk->bgl				= GF_BGL_BglIniAlloc( HEAPID_SCRATCH );
	wk->proc			= proc;
	scratch_call			= (SCRATCH_CALL_WORK*)PROC_GetParentWork( proc );
	wk->sv				= scratch_call->sv;
	wk->type			= scratch_call->type;
	wk->p_ret_work		= &scratch_call->ret_work;
	wk->config			= SaveData_GetConfig( wk->sv );			//コンフィグポインタを取得
	wk->p_item_no		= &scratch_call->item_no[0];
	wk->p_item_num		= &scratch_call->item_num[0];

	Scratch_CardPokeClear( wk );

	//商品クリア
	for( i=0; i < SCRATCH_CARD_ATARI_NUM ;i++ ){
		wk->p_item_no[i]	= 0;
		wk->p_item_num[i]	= 0;
	}

	//パレットリソース切り替え初期値セット
	wk->pltt_res_no = 0;

	Scratch_InitSub2( wk );

	(*seq) = SEQ_GAME_INIT;
	OS_Printf( "(*seq) = %d\n", (*seq) );

	Snd_DataSetByScene( SND_SCENE_SUB_SCRATCH, 0, 0 );

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
PROC_RESULT ScratchProc_Main( PROC * proc, int * seq )
{
	SCRATCH_WORK* wk  = PROC_GetWork( proc );
	//OS_Printf( "(*seq) = %d\n", (*seq) );

	switch( *seq ){

	//-----------------------------------
	//初期化
	case SEQ_GAME_INIT:
		if( Seq_GameInit(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_CARD_IN );
		}
		break;

	//-----------------------------------
	//カードイン
	case SEQ_GAME_CARD_IN:
		if( Seq_GameCardIn(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_CARD_SEL );			//カード選択へ
		}
		break;

	//-----------------------------------
	//カード選択中
	case SEQ_GAME_CARD_SEL:
		if( Seq_GameCardSel(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_CARD_OUT );			//カードアウトへ
		}
		break;

	//-----------------------------------
	//カードアウト
	case SEQ_GAME_CARD_OUT:
		if( Seq_GameCardOut(wk) == TRUE ){
			if( wk->card_num >= SCRATCH_CARD_NUM ){
				wk->card_num = 0;

				Scratch_CardDelete( wk );					//下：カードOBJ削除
				//ScratchClact_ResButtonChg( &wk->scratch_clact );	//リソース切り替え(card→button)
				Scratch_WakuAdd( wk );						//下：ワクOBJ追加
				//Scratch_ButtonAdd( wk );					//下：ボタンOBJ追加
				Scratch_AtariAdd( wk );						//下：当たりポケモンOBJ追加

				NextSeq( wk, seq, SEQ_GAME_CARD_TOUCH_IN );	//カード削りインへ
			}else{
				NextSeq( wk, seq, SEQ_GAME_CARD_IN );		//カードインへ
			}
		}
		break;

	//-----------------------------------
	//カード削りイン
	case SEQ_GAME_CARD_TOUCH_IN:
		if( Seq_GameCardTouchIn(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_CARD_TOUCH );		//カード削りへ
		}
		break;

	//-----------------------------------
	//カード削り中
	case SEQ_GAME_CARD_TOUCH:
		if( Seq_GameCardTouch(wk) == TRUE ){
			if( wk->card_num >= SCRATCH_CARD_NUM ){
				NextSeq( wk, seq, SEQ_GAME_END );			//終了へ
			}else{
				NextSeq( wk, seq, SEQ_GAME_CARD_TOUCH_OUT );//カード削りアウトへ
			}
		}
		break;

	//-----------------------------------
	//カード削りアウト
	case SEQ_GAME_CARD_TOUCH_OUT:
		if( Seq_GameCardTouchOut(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_CARD_TOUCH_IN );		//カード削りインへ
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

	CLACT_Draw( wk->scratch_clact.ClactSet );		//セルアクター常駐関数

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
PROC_RESULT ScratchProc_End( PROC * proc, int * seq )
{
	int i;
	SCRATCH_WORK* wk = PROC_GetWork( proc );

	StopTP();

	*(wk->p_ret_work) = wk->csr_pos;					//戻り値格納ワークへ代入(カーソル位置)

	OS_Printf( "*(wk->p_ret_work) = %d\n", *(wk->p_ret_work) );

	DellVramTransferManager();

	ScratchCommon_Delete( wk );							//削除処理

	PROC_FreeWork( proc );								//ワーク開放

	sys_VBlankFuncChange( NULL, NULL );					//VBlankセット
	sys_DeleteHeap( HEAPID_SCRATCH );

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
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameInit( SCRATCH_WORK* wk )
{
	switch( wk->sub_seq ){

	//ブラックイン
	case 0:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
						WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC*3, HEAPID_SCRATCH );

		wk->sub_seq++;
		break;
		
	//フェード終了待ち
	case 1:
		if( WIPE_SYS_EndCheck() == TRUE ){
			wk->card_num = 0;
			return TRUE;
		}
		break;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	ゲームカードイン
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameCardIn( SCRATCH_WORK* wk )
{
	int i;

	enum{
		SEQ_SUB_START = 0,
		SEQ_SUB_CARD_IN,
		SEQ_SUB_CARD_IN_END,
	};

	switch( wk->sub_seq ){

	//カードをインする位置に配置
	case SEQ_SUB_START:
		wk->counter = SCRATCH_CARD_IN_OFFSET_X;

		for( i=0; i < SCRATCH_D_CARD_MAX ;i++ ){
			ScratchObj_SetObjPos(	wk->p_d_card[i], (d_card_pos[i].x + wk->counter), 
									d_card_pos[i].y );
			ScratchObj_Priority( wk->p_d_card[i], (SCRATCH_OBJ_PRI_L+i) );
		}

		//「何枚目のカードを選んでください」
		ScratchTalkWinPut( &wk->bmpwin[BMPWIN_SELECT], CONFIG_GetWindowType(wk->config) );

		Scratch_CardSelectMsg( wk );
		GF_Disp_GXS_VisibleControl( SCRATCH_U_MASK_MSG, VISIBLE_ON );		//上メッセージオン

		wk->counter2 = (SCRATCH_D_CARD_MAX - 1);
		Snd_SePlay( SE_D_CARD_IN );
		wk->sub_seq = SEQ_SUB_CARD_IN;
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//カードイン
	case SEQ_SUB_CARD_IN:
		wk->counter += SCRATCH_CARD_IN_SPD;
		ScratchObj_SetObjPos(	wk->p_d_card[wk->counter2], 
								(d_card_pos[wk->counter2].x + wk->counter), 
								d_card_pos[wk->counter2].y );

		if( wk->counter >= 0 ){
			//全てのカードを処理したら
			if( wk->counter2 == 0 ){
				wk->sub_seq = SEQ_SUB_CARD_IN_END;
			}else{
				Snd_SePlay( SE_D_CARD_IN );
				wk->counter2--;												//次のカードへ
				wk->counter = SCRATCH_CARD_IN_OFFSET_X;
			}
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//カードイン終了
	case SEQ_SUB_CARD_IN_END:
		wk->counter = 0;
		return TRUE;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	ゲームカード選択中
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameCardSel( SCRATCH_WORK* wk )
{
	u32 tp_x,tp_y;
	int i,index;

	enum{
		SEQ_SUB_START = 0,
		SEQ_SUB_CARD_SEL,
		SEQ_SUB_CARD_SEL_END,
	};

	switch( wk->sub_seq ){

	//
	case SEQ_SUB_START:
		//ScratchObj_Vanish( wk->p_d_win, SCRATCH_VANISH_OFF );					//オン
		//Scratch_CardNumMsg( wk );
		GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_MSG, VISIBLE_ON );			//下メッセージオン
		wk->time_wait = SCRATCH_START_MSG_DEL_WAIT;
		wk->sub_seq = SEQ_SUB_CARD_SEL;
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//カード選択中
	case SEQ_SUB_CARD_SEL:

		//時間でメッセージ消しておく
		if( wk->time_wait == 0 ){
			GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_MSG, VISIBLE_OFF );		//下メッセージオフ
			ScratchObj_Vanish( wk->p_d_win, SCRATCH_VANISH_ON );				//オフ
		}else{
			wk->time_wait--;
		}

		index = GF_TP_RectHitCont( (const RECT_HIT_TBL*)card_hit_tbl );

		//if( GF_TP_SingleHitCont((const RECT_HIT_TBL*)card_hit_tbl) == TRUE ){
		if( index != RECT_HIT_NONE ){
			GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_MSG, VISIBLE_OFF );		//下メッセージオフ
			ScratchObj_Vanish( wk->p_d_win, SCRATCH_VANISH_ON );				//オフ
			Snd_SePlay( SE_D_BUTTON_TOUCH );

			wk->card_col[wk->card_num] = index;		//選んだカードの場所(色ナンバーを格納)
			wk->card_num++;
			OS_Printf( "カードにタッチしました = %d\n", index );
			OS_Printf( "0=左上赤、1=右上青、2=左下黄色、3=右下緑\n" );

			//タッチパネルに触れているならその座標取得(ベタ入力)
			GF_TP_GetPointCont( &tp_x, &tp_y );
			OS_Printf( "tp_x = %d\ttp_y = %d\n", tp_x, tp_y );

			BmpTalkWinClear( &wk->bmpwin[BMPWIN_SELECT], WINDOW_TRANS_ON );
			GF_Disp_GXS_VisibleControl( SCRATCH_U_MASK_MSG, VISIBLE_OFF );		//上メッセージオフ
			wk->sub_seq = SEQ_SUB_CARD_SEL_END;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//カード選択終了
	case SEQ_SUB_CARD_SEL_END:
		return TRUE;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	ゲームカードアウト
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameCardOut( SCRATCH_WORK* wk )
{
	int i,pos,now_x,now_y;

	enum{
		SEQ_SUB_START = 0,
		SEQ_SUB_CARD_AFF,
		SEQ_SUB_CARD_OUT,
		SEQ_SUB_CARD_OUT_END,
	};

	//どのカードを選んだか
	pos = wk->card_col[wk->card_num-1];
					
	switch( wk->sub_seq ){

	//
	case SEQ_SUB_START:
		wk->scale_fx.x = SCRATCH_CARD_AFF_DEFAULT;		//等倍
		wk->scale_fx.y = SCRATCH_CARD_AFF_DEFAULT;
		wk->scale_fx.z = SCRATCH_CARD_AFF_DEFAULT;
		wk->counter = 0;
		wk->move_num= 0;
		OS_Printf( "どのカードか pos = %d\n", pos );
		ScratchObj_Priority( wk->p_d_card[pos], SCRATCH_OBJ_PRI_H );

		wk->sub_seq = SEQ_SUB_CARD_AFF;
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//拡大
	case SEQ_SUB_CARD_AFF:

		//カードを中心に移動
		if( Scratch_CardCenterMove(wk,pos) == TRUE ){
			//
		}

		wk->scale_fx.x += SCRATCH_CARD_AFF_ADD;
		wk->scale_fx.y += SCRATCH_CARD_AFF_ADD;
		wk->scale_fx.z += SCRATCH_CARD_AFF_ADD;

		wk->counter++;
		if( (wk->counter % SCRATCH_CARD_AFF_WAIT) == 0 ){

			//選んだカードにする
			ScratchObj_SetScaleAffineTbl( wk->p_d_card[pos], &wk->scale_fx );
		}

		//カードの最大拡大
		if( wk->scale_fx.x == SCRATCH_CARD_AFF_MAX ){
			wk->counter = 0;

			wk->scale_fx.x = 0x1000;
			wk->scale_fx.y = 0x1000;
			wk->scale_fx.z = 0x1000;

			Snd_SePlay( SE_D_CARD_OUT );
			wk->sub_seq = SEQ_SUB_CARD_OUT;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//カードアウト
	case SEQ_SUB_CARD_OUT:

		//カードを中心に移動
		//if( Scratch_CardCenterMove(wk,pos) == TRUE ){
			//
		//}

		wk->counter += SCRATCH_CARD_OUT_SPD;
		for( i=0; i < SCRATCH_D_CARD_MAX ;i++ ){
			ScratchObj_GetObjPos( wk->p_d_card[i], &now_x, &now_y );	//現在の座標取得
			OS_Printf( "i = %d, now_x = %d\n", i, now_x );
			ScratchObj_SetObjPos(	wk->p_d_card[i], (now_x + SCRATCH_CARD_OUT_SPD), 
									now_y );
			//ScratchObj_SetObjPos(	wk->p_d_card[i], (d_card_pos[i].x + wk->counter), 
			//						d_card_pos[i].y );
			
			//小さくする
			//if( now_x >= 368 ){
			//	ScratchObj_SetScaleAffineTbl( wk->p_d_card[pos], &wk->scale_fx );
			//}
		}

		if( wk->counter >= SCRATCH_CARD_OUT_OFFSET_X ){
		//if( wk->counter >= SCRATCH_CARD_OUT_OFFSET_X + 128 ){		//拡大分(128)
			//ScratchObj_SetScaleAffine2( wk->p_d_card[0], 0 );
			//ScratchObj_SetScaleAffine( wk->p_d_card[0], 5 );
			

			//ScratchObj_SetScaleAffineTbl( wk->p_d_card[pos], &wk->scale_fx );
			ScratchObj_SetScaleAffine( wk->p_d_card[pos], 0 );
			wk->sub_seq = SEQ_SUB_CARD_OUT_END;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//カードアウト終了
	case SEQ_SUB_CARD_OUT_END:
		wk->counter = 0;
		return TRUE;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	ゲームカード削りイン
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameCardTouchIn( SCRATCH_WORK* wk )
{
	int i,pos;

	enum{
		SEQ_SUB_CARD_TOUCH_IN_START = 0,
		SEQ_SUB_CARD_TOUCH_IN_START2,
		SEQ_SUB_CARD_TOUCH_IN,
		SEQ_SUB_CARD_TOUCH_IN_END,
	};

	switch( wk->sub_seq ){

	//カードをインする位置に配置
	case SEQ_SUB_CARD_TOUCH_IN_START:
		//キャラクターデータを元に戻す
		Scratch_Touch_1_5( wk );
		GF_BGL_LoadCharacter( wk->bgl, SCRATCH_FRAME_D_TOUCH, wk->buf, wk->p_chardata->szByte, 0 );
		GF_BGL_LoadScreenReq( wk->bgl, SCRATCH_FRAME_D_TOUCH );

		wk->counter2 = SCRATCH_CARD_TOUCH_IN_OFFSET_X;
		GF_BGL_ScrollSet( wk->bgl, SCRATCH_FRAME_U_YAKU, GF_BGL_SCROLL_X_SET, wk->counter2 );
		GF_BGL_ScrollSet( wk->bgl, SCRATCH_FRAME_D_TOUCH, GF_BGL_SCROLL_X_SET, wk->counter2 );
		GF_BGL_ScrollSet( wk->bgl, SCRATCH_FRAME_D_SUB, GF_BGL_SCROLL_X_SET, wk->counter2 );

		GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_TOUCH, VISIBLE_ON );			//削り面オン
		GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_SUB, VISIBLE_ON );			//カード面オン
		GF_Disp_GXS_VisibleControl( SCRATCH_U_MASK_YAKU, VISIBLE_ON );			//役オン

		wk->counter = SCRATCH_CARD_IN_OFFSET_X;

		//イージー決定
		wk->mode[ wk->card_num ] = SCRATCH_CARD_MODE_EASY;						//イージカード

		//クリア
		for( i=0; i < SCRATCH_TOUCH_SIZE_MAX ;i++ ){
			wk->touch_flag[i] = 0;
		}

		//削ったカードの数をクリア
		wk->kezuri_card_num = 0;

		//カードのポケモン決定
		Scratch_CardPokeClear( wk );
		Scratch_CardPokeSet( wk, wk->mode[ wk->card_num ] );

		//役を決定
		Scratch_YakuSet( wk, wk->mode[ wk->card_num ] );

		//銀箔に触れたフラグクリア
		Scratch_BlockFlagClear( wk );

		//触れた位置カウントをクリア
		Scratch_BlockCountClear( wk );

		wk->sub_seq = SEQ_SUB_CARD_TOUCH_IN_START2;
		break;

	case SEQ_SUB_CARD_TOUCH_IN_START2:
		//上：ポケモンOBJ表示
		for( i=0; i < SCRATCH_U_POKE_MAX ;i++ ){
			ScratchObj_SetObjPos(	wk->p_u_poke[i], 
							(SCRATCH_U_POKE_X + wk->counter),
							(SCRATCH_U_POKE_Y + (i * SCRATCH_U_POKE_WIDTH_Y)) );
			ScratchObj_Vanish( wk->p_u_poke[i], SCRATCH_VANISH_OFF );		//表示
		}

		//下：ワクOBJ表示
		for( i=0; i < SCRATCH_CARD_ATARI_NUM ;i++ ){
			//使用パレットを元に戻す
			ScratchObj_PaletteNoChg( wk->p_d_waku[i], 1 );
		}

		//クリア
		wk->pltt_res_no		= 0;
		wk->waku_flag		= 0;
		wk->kezuri_card[0]	= DUMMY_KEZURI_CARD1;
		wk->kezuri_card[1]	= DUMMY_KEZURI_CARD2;
		wk->kezuri_card[2]	= DUMMY_KEZURI_CARD3;

		//下：ポケモンOBJ表示
		for( i=0; i < SCRATCH_D_POKE_MAX ;i++ ){
			ScratchObj_SetObjPos(	wk->p_d_poke[i], (d_poke_pos[i].x + wk->counter), 
									d_poke_pos[i].y );
			ScratchObj_Vanish( wk->p_d_poke[i], SCRATCH_VANISH_OFF );		//表示
			ScratchObj_AnmChg( wk->p_d_poke[i], wk->card_poke[i] );			//配置ポケモンに変更
		}

		//カードのスクリーンのパレット変更(u=4,5,6,7パレット、d=1,2,3,4パレット)
		ScrPalChg( wk, SCRATCH_FRAME_D_SUB, (wk->card_col[ wk->card_num ] + 4), 32, 24 );
		ScrPalChg( wk, SCRATCH_FRAME_U_YAKU, (wk->card_col[ wk->card_num ] + 1), 32, 24 );

		Snd_SePlay( SE_D_CARD_IN );
		wk->sub_seq = SEQ_SUB_CARD_TOUCH_IN;
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//カードイン
	case SEQ_SUB_CARD_TOUCH_IN:
		GF_BGL_ScrollSet( wk->bgl, SCRATCH_FRAME_U_YAKU, GF_BGL_SCROLL_X_SET, wk->counter2 );
		GF_BGL_ScrollSet( wk->bgl, SCRATCH_FRAME_D_TOUCH, GF_BGL_SCROLL_X_SET, wk->counter2 );
		GF_BGL_ScrollSet( wk->bgl, SCRATCH_FRAME_D_SUB, GF_BGL_SCROLL_X_SET, wk->counter2 );
		wk->counter2 += SCRATCH_CARD_TOUCH_IN_SPD;

		if( wk->counter >= 0 ){
			wk->sub_seq = SEQ_SUB_CARD_TOUCH_IN_END;
		}else{
			//上：ポケモンOBJ表示
			for( i=0; i < SCRATCH_U_POKE_MAX ;i++ ){
				ScratchObj_SetObjPos(	wk->p_u_poke[i], 
							(SCRATCH_U_POKE_X + wk->counter),
							(SCRATCH_U_POKE_Y + (i * SCRATCH_U_POKE_WIDTH_Y)) );
			}

			//下：ポケモンOBJ表示
			for( i=0; i < SCRATCH_D_POKE_MAX ;i++ ){
				ScratchObj_SetObjPos(	wk->p_d_poke[i], (d_poke_pos[i].x + wk->counter), 
										d_poke_pos[i].y );
			}
		}

		wk->counter += SCRATCH_CARD_IN_SPD2;
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//カードイン終了
	case SEQ_SUB_CARD_TOUCH_IN_END:
		wk->counter	= 0;
		wk->counter2= 0;
		return TRUE;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	ゲームカード削り中
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameCardTouch( SCRATCH_WORK* wk )
{
	int i,index;
	u32 ret,color;

	enum{
		SEQ_SUB_START = 0,
		SEQ_SUB_START_SUB,
		SEQ_SUB_MSG_WAIT,
		SEQ_SUB_TYPE_SEL,

		SEQ_SUB_GIN_OK_ATARI_MOSAIC_WAIT,
		SEQ_SUB_GIN_OK_ATARI_MOSAIC_WAIT2,

		SEQ_SUB_GIN_OK_ATARI_MSG_WAIT,
		SEQ_SUB_GIN_OK_HAZURE_MSG_WAIT,
		SEQ_SUB_GIN_OK_GIN_ALL_CLEAR_WAIT,
		SEQ_SUB_NEXT_WAIT,
		SEQ_SUB_YAMERU_WAIT,
		SEQ_SUB_YAMERU_WAIT_NO,
	};

	switch( wk->sub_seq ){

	case SEQ_SUB_START:
		if( wk->card_num == (SCRATCH_CARD_NUM - 1) ){
			GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_NEXT] );
			Scratch_YameruMsg( wk );
			ScratchObj_SetObjPos( wk->p_d_button, d_yameru_button_pos.x, d_yameru_button_pos.y );
			ScratchObj_AnmChg( wk->p_d_button, ANM_BUTTON_YAMERU );				//やめるに変更
		}else{
			GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_YAMERU] );
			Scratch_NextMsg( wk );
		}

		wk->time_wait = SCRATCH_START_MSG_DEL_WAIT;
		ScratchObj_Vanish( wk->p_d_win, SCRATCH_VANISH_OFF );					//オン
		Scratch_StartMsg( wk );

		//役を表示
		for( i=0; i < SCRATCH_ATARI_MAX ;i++ ){
			Scratch_YakuMsg( wk, i );
		}

		//つぎのカードへ(やめる)ボタン
		ScratchObj_Vanish( wk->p_d_button, SCRATCH_VANISH_OFF );				//オン
		ScratchObj_SetAnmFlag( wk->p_d_button, 0 );								//ボタンアニメOFF

		Snd_SePlay( SE_D_GAME_START );
		wk->sub_seq = SEQ_SUB_START_SUB;
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	case SEQ_SUB_START_SUB:
		GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_MSG, VISIBLE_ON );			//下メッセージオン
		GF_Disp_GXS_VisibleControl( SCRATCH_U_MASK_MSG, VISIBLE_ON );			//上メッセージオン
		wk->sub_seq = SEQ_SUB_MSG_WAIT;
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//メッセージ表示待ち
	case SEQ_SUB_MSG_WAIT:

		//時間でメッセージ消しておく
		if( wk->time_wait == 0 ){
			ScratchObj_Vanish( wk->p_d_win, SCRATCH_VANISH_ON );				//オフ
			GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_START], FBMP_COL_NULL );
			//GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_ATARI], FBMP_COL_NULL );
			//GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_HAZURE], FBMP_COL_NULL );
			GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_START] );
			//GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_ATARI] );
			//GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_HAZURE] );
			
			wk->sub_seq = SEQ_SUB_TYPE_SEL;
		}else{
			wk->time_wait--;
		}

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//タイプを選択中
	case SEQ_SUB_TYPE_SEL:
#if 0
	1:	キャラデータをバッファにコピー
	2:	タッチパネルで触れている座標取得
	3:	2)の位置のキャラデータのドット情報をクリア
	4:	2),3)の繰り返し

	2の位置から縦、横何ドットを対応させる

	触っているドットの位置のみバッファを書き直して転送にする
#endif

#ifdef PM_DEBUG

		//BgCheck( wk );
		
		//ワクを消すデバックキー
		if( sys.cont & PAD_BUTTON_L ){
			GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_TOUCH, VISIBLE_OFF );		//削り面オフ
		}else if( sys.cont & PAD_BUTTON_R ){
			GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_TOUCH, VISIBLE_ON );		//削り面オン
		}
#endif

		//サンプリング情報取得
		//MainTP( &wk->tpData, TP_BUFFERING_JUST, 1 );
		MainTP( &wk->tpData, TP_BUFFERING_JUST, 2 );

		index = GF_TP_RectHitCont( (const RECT_HIT_TBL*)block_hit_tbl );
		//if( (index != RECT_HIT_NONE) && (index != 4) ){
		if( index != RECT_HIT_NONE ){

			//タッチパネルに触れているならその座標取得(ベタ入力)
			//GF_TP_GetPointCont( &wk->tp_x, &wk->tp_y );			//現在座標

			//まだ銀箔ブロックに触れる時
			if( Scratch_BlockFlagOnNumGet(wk) < SCRATCH_CARD_KEZURI_NUM ){
				wk->block_flag[index] = 1;
			}

			//触れたブロックの銀箔だったら
			if( wk->block_flag[index] == 1 ){

				//触れたフラグを立てる
				Scratch_TouchFlagSet( wk );

				//触れたフラグチェック
				Scratch_TouchFlagCheck( wk, index );
			
				//tpDataに格納された座標のキャラクタデータを書き換える
				Scratch_HoseiTouch2( wk, index );

				//削り音を再生していなかったら再生
				if( Snd_SePlayCheck(SE_D_CARD_KEZURI) == FALSE ){
					Snd_SePlay( SE_D_CARD_KEZURI );
				}

				//タッチパネルに触れている座標を取得してキャラクターデータを書き換える
				//Scratch_Touch_2( wk, wk->tp_x, wk->tp_y );
			}

			//触れたフラグチェック
			//Scratch_TouchFlagCheck( wk, index );

			//当たりがそろったら
			if( Scratch_AtariCheck(wk) == TRUE ){

				Snd_SeStopBySeqNo( SE_D_CARD_KEZURI, 0 );
				ScratchObj_Vanish( wk->p_d_win, SCRATCH_VANISH_OFF );	//表示

				//商品をセット
				wk->p_item_no[wk->card_num]	= wk->atari_item[wk->atari_poke];

				//「きんのたま」の時はアイテム数を1にする
				if( wk->atari_item[wk->atari_poke] == ITEM_KINNOTAMA ){
					wk->p_item_num[wk->card_num]= 1;
				}else{
					wk->p_item_num[wk->card_num]= 3;
				}

				//メタモンのモザイクエフェクト初期化
				if( Scratch_MosaicEffInit(wk) == TRUE ){
					wk->time_wait = SCRATCH_RESULT_MOSAIC_WAIT;
				}else{
					wk->time_wait = 0;
				}

				Scratch_AtariMsg( wk );									//当たりメッセージ
				wk->sub_seq = SEQ_SUB_GIN_OK_ATARI_MOSAIC_WAIT;
				return FALSE;
			}

			//当たりがそろっていなくて、全て削った時
			if( wk->kezuri_card_num >= SCRATCH_CARD_KEZURI_NUM	){

				Snd_SeStopBySeqNo( SE_D_CARD_KEZURI, 0 );
				wk->time_wait = SCRATCH_RESULT_MSG_DEL_WAIT;
				ScratchObj_Vanish( wk->p_d_win, SCRATCH_VANISH_OFF );	//表示

				Scratch_HazureMsg( wk );								//外れメッセージ
				wk->sub_seq = SEQ_SUB_GIN_OK_HAZURE_MSG_WAIT;
				return FALSE;
			}

		//タッチしていない時
		}else{
			Snd_SeStopBySeqNo( SE_D_CARD_KEZURI, 0 );					//削りループ音を止める
		}
	
		//リーチの時(画面にタッチしていない時も処理する)
		if( wk->waku_flag == 1 ){
			Scratch_PlttNoResChg( wk );		//ウェイト計算してパレットナンバー、リソース変更
		}

		//3枚目のカードを遊んでいる時
		if( wk->card_num == (SCRATCH_CARD_NUM - 1) ){
			//やめるをタッチしたか
			index = GF_TP_RectHitTrg( (const RECT_HIT_TBL*)yameru_hit_tbl );
			if( index != RECT_HIT_NONE ){
				ScratchObj_SetAnmFlag( wk->p_d_button, 1 );		//ボタンアニメON
				Scratch_KakuninMsg( wk );						//スクラッチをやめますか？
				Snd_SePlay( SE_D_BUTTON_TOUCH );
				ScratchObj_AnmChg( wk->p_d_button, ANM_BUTTON_YAMERU );		//やめるに変更
				Scratch_AllocTouchSub( wk );
				Scratch_InitTouchSub( wk );
				wk->sub_seq = SEQ_SUB_YAMERU_WAIT;
				break;
			}
		}else{
			//つぎのカードへをタッチしたか
			index = GF_TP_RectHitTrg( (const RECT_HIT_TBL*)next_hit_tbl );

			if( index != RECT_HIT_NONE ){
				ScratchObj_SetAnmFlag( wk->p_d_button, 1 );		//ボタンアニメON
				Scratch_KakuninMsg( wk );						//スクラッチをやめますか？
				Snd_SePlay( SE_D_BUTTON_TOUCH );
				ScratchObj_AnmChg( wk->p_d_button, ANM_BUTTON_NEXT );		//つぎのカードへに変更
				Scratch_AllocTouchSub( wk );
				Scratch_InitTouchSub( wk );
				wk->sub_seq = SEQ_SUB_NEXT_WAIT;
				break;
			}
		}

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//モザイクウェイト
	case SEQ_SUB_GIN_OK_ATARI_MOSAIC_WAIT:

		if( wk->time_wait == 0 ){
			//メタモンのモザイクエフェクト終了待ち
			if( Scratch_MosaicEffMain(wk,0) == FALSE ){
				wk->sub_seq = SEQ_SUB_GIN_OK_ATARI_MOSAIC_WAIT2;
			}
		}else{
			wk->time_wait--;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//モザイクウェイト2
	case SEQ_SUB_GIN_OK_ATARI_MOSAIC_WAIT2:

		//メタモンのモザイクエフェクト終了待ち
		if( Scratch_MosaicEffMain(wk,1) == FALSE ){
			//wk->time_wait = SCRATCH_RESULT_MSG_DEL_WAIT;
			wk->time_wait = 0;
			wk->sub_seq = SEQ_SUB_GIN_OK_ATARI_MSG_WAIT;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//当たりメッセージ表示
	case SEQ_SUB_GIN_OK_ATARI_MSG_WAIT:

		//当たりウィンドウがアニメーションしていたら抜ける
		if( ScratchObj_AnmActiveCheck(wk->p_d_atari_win) == TRUE ){
			break;
		}

		//時間でメッセージ消しておく
		if( wk->time_wait == 0 ){

			for( i=0; i < SCRATCH_CARD_ATARI_NUM ;i++ ){
				ScratchObj_Vanish( wk->p_d_atari[i], SCRATCH_VANISH_ON );		//非表示
			}
			
			ScratchObj_Vanish( wk->p_d_atari_win, SCRATCH_VANISH_ON );			//非表示
			//GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_ATARI, VISIBLE_OFF );	//下当たりwin off
			Scratch_SeqSubNextCard( wk );
			Scratch_SeqSubGinOk( wk );
			return TRUE;
		}else{
			wk->time_wait--;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//はずれメッセージ表示
	case SEQ_SUB_GIN_OK_HAZURE_MSG_WAIT:

		//時間でメッセージ消しておく
		if( wk->time_wait == 0 ){
			GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_TOUCH, VISIBLE_OFF );	//削り面オフ
			Scratch_SeqSubNextCard( wk );
			Scratch_SeqSubGinOk( wk );
			wk->time_wait = SCRATCH_GIN_ALL_CLEAR_WAIT;
			wk->sub_seq = SEQ_SUB_GIN_OK_GIN_ALL_CLEAR_WAIT;
		}else{
			wk->time_wait--;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//はずれで銀箔を全てクリアしたウェイト
	case SEQ_SUB_GIN_OK_GIN_ALL_CLEAR_WAIT:
		if( wk->time_wait == 0 ){
			return TRUE;
		}else{
			wk->time_wait--;
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//つぎのカードへウェイト
	case SEQ_SUB_NEXT_WAIT:
		ret = TOUCH_SW_MainMC( wk->touch_subwin );

		//何もなし
		//if( ret == TOUCH_SW_RET_NORMAL ){
			//
		//}

		//「はい」押した瞬間
		//if( ret == TOUCH_SW_RET_YES_TOUCH ){
		if( ret == TOUCH_SW_RET_YES ){
			ScratchObj_SetAnmFlag( wk->p_d_button, 0 );		//ボタンアニメOFF
			BmpMenuWinClear( &wk->bmpwin[BMPWIN_TALK], WINDOW_TRANS_ON );
			GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_TALK] );
			Scratch_SeqSubNextCard( wk );
			Scratch_FreeTouchSub( wk );
			return TRUE;

		//「いいえ」押した瞬間
		//}else if( ret == TOUCH_SW_RET_NO_TOUCH ){
		}else if( ret == TOUCH_SW_RET_NO ){
			ScratchObj_SetAnmFlag( wk->p_d_button, 0 );		//ボタンアニメOFF
			BmpMenuWinClear( &wk->bmpwin[BMPWIN_TALK], WINDOW_TRANS_ON );
			GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_TALK] );
			Scratch_FreeTouchSub( wk );
			wk->sub_seq = SEQ_SUB_YAMERU_WAIT_NO;
		}

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//やめるウェイト
	case SEQ_SUB_YAMERU_WAIT:
		ret = TOUCH_SW_MainMC( wk->touch_subwin );

		//何もなし
		//if( ret == TOUCH_SW_RET_NORMAL ){
			//
		//}

		//「はい」押した瞬間
		//if( ret == TOUCH_SW_RET_YES_TOUCH ){
		if( ret == TOUCH_SW_RET_YES ){
			ScratchObj_SetAnmFlag( wk->p_d_button, 0 );		//ボタンアニメOFF
			BmpMenuWinClear( &wk->bmpwin[BMPWIN_TALK], WINDOW_TRANS_ON );
			GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_TALK] );
			Scratch_SeqSubYameruCard( wk );
			Scratch_FreeTouchSub( wk );
			return TRUE;

		//「いいえ」押した瞬間
		//}else if( ret == TOUCH_SW_RET_NO_TOUCH ){
		}else if( ret == TOUCH_SW_RET_NO ){
			ScratchObj_SetAnmFlag( wk->p_d_button, 0 );		//ボタンアニメOFF
			BmpMenuWinClear( &wk->bmpwin[BMPWIN_TALK], WINDOW_TRANS_ON );
			GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_TALK] );
			Scratch_FreeTouchSub( wk );
			wk->sub_seq = SEQ_SUB_YAMERU_WAIT_NO;
		}

		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//やめるウェイト「いいえ」
	case SEQ_SUB_YAMERU_WAIT_NO:
		if( GF_TP_GetCont() == FALSE ){			//一度画面から離してから次へ進む
			wk->sub_seq = SEQ_SUB_TYPE_SEL;
		}
		break;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	ゲームカード削りアウト
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameCardTouchOut( SCRATCH_WORK* wk )
{
	int i,pos;

	enum{
		SEQ_SUB_CARD_TOUCH_OUT_START = 0,
		SEQ_SUB_CARD_TOUCH_OUT,
		SEQ_SUB_CARD_TOUCH_OUT_END,
	};

	//どのカードを選んだか
	pos = wk->card_col[wk->card_num-1];
	OS_Printf( "pos = %d\n", pos );
					
	switch( wk->sub_seq ){

	//
	case SEQ_SUB_CARD_TOUCH_OUT_START:
		GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_MSG, VISIBLE_OFF );		//下メッセージオフ
		GF_Disp_GXS_VisibleControl( SCRATCH_U_MASK_MSG, VISIBLE_OFF );		//上メッセージオフ

		//つぎのカードへ(やめる)ボタン
		ScratchObj_Vanish( wk->p_d_button, SCRATCH_VANISH_ON );				//オフ

		wk->counter	= 0;
		wk->counter2= 0;

		Snd_SePlay( SE_D_CARD_OUT );
		wk->sub_seq = SEQ_SUB_CARD_TOUCH_OUT;
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//カードアウト
	case SEQ_SUB_CARD_TOUCH_OUT:
		GF_BGL_ScrollSet( wk->bgl, SCRATCH_FRAME_U_YAKU, GF_BGL_SCROLL_X_SET, wk->counter2 );
		GF_BGL_ScrollSet( wk->bgl, SCRATCH_FRAME_D_TOUCH, GF_BGL_SCROLL_X_SET, wk->counter2 );
		GF_BGL_ScrollSet( wk->bgl, SCRATCH_FRAME_D_SUB, GF_BGL_SCROLL_X_SET, wk->counter2 );
		wk->counter2 += SCRATCH_CARD_TOUCH_OUT_SPD;

		//if( wk->counter2 <= SCRATCH_CARD_TOUCH_OUT_OFFSET_X ){
		if( wk->counter >= SCRATCH_CARD_OUT_OFFSET_X ){
			wk->sub_seq = SEQ_SUB_CARD_TOUCH_OUT_END;
		}else{
			//上：ポケモンOBJ表示
			for( i=0; i < SCRATCH_U_POKE_MAX ;i++ ){
				ScratchObj_SetObjPos(	wk->p_u_poke[i], 
						(SCRATCH_U_POKE_X + wk->counter),
						(SCRATCH_U_POKE_Y + (i * SCRATCH_U_POKE_WIDTH_Y)) );
			}

#if 0
			//下：ワクOBJ表示
			for( i=0; i < wk->kezuri_card_num ;i++ ){
				ScratchObj_SetObjPos(wk->p_d_waku[i], 
					(d_poke_pos[ wk->kezuri_card[i] ].x + wk->counter + SCRATCH_GIN_WAKU_OFFSET_X), 
					d_poke_pos[ wk->kezuri_card[i] ].y + SCRATCH_GIN_WAKU_OFFSET_Y );

				ScratchObj_Vanish( wk->p_d_waku[i], SCRATCH_VANISH_ON );		//非表示
			}
#else
			for( i=0; i < SCRATCH_CARD_ATARI_NUM ;i++ ){
				ScratchObj_Vanish( wk->p_d_waku[i], SCRATCH_VANISH_ON );		//非表示
			}
#endif

			//下：ポケモンOBJ表示
			for( i=0; i < SCRATCH_D_POKE_MAX ;i++ ){
				ScratchObj_SetObjPos(	wk->p_d_poke[i], (d_poke_pos[i].x + wk->counter), 
										d_poke_pos[i].y );
			}
		}

		wk->counter += SCRATCH_CARD_OUT_SPD;
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//カードアウト終了
	case SEQ_SUB_CARD_TOUCH_OUT_END:
		wk->counter	= 0;
		wk->counter2= 0;
		return TRUE;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：終了へ
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameEnd( SCRATCH_WORK* wk )
{
	int i;

	switch( wk->sub_seq ){

	case 0:
		wk->wait = SCRATCH_END_WAIT;
		wk->sub_seq++;
		break;

	//フェードアウト
	case 1:
		wk->wait--;
		if( wk->wait == 0 ){
			BmpTalkWinClear( &wk->bmpwin[BMPWIN_TALK], WINDOW_TRANS_ON );
			WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
					WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_SCRATCH );
			wk->sub_seq++;
		}
		break;

	//フェード終了待ち
	case 2:
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
 * @brief	下画面ワク追加
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_WakuAdd( SCRATCH_WORK* wk )
{
	int i;

	for( i=0; i < SCRATCH_CARD_ATARI_NUM ;i++ ){

		wk->p_d_waku[i] = ScratchObj_Create(&wk->scratch_clact, SCRATCH_ID_OBJ_D_BUTTON, 
											ANM_GIN_WAKU, d_poke_pos[i].x, d_poke_pos[i].y, 
											DISP_MAIN,
											SCRATCH_OBJ_BG_PRI_L, 
											(SCRATCH_OBJ_PRI_N + 1) );
											//SCRATCH_OBJ_PRI_H );
		ScratchObj_Vanish( wk->p_d_waku[i], SCRATCH_VANISH_ON );		//非表示
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	下画面当たりポケモン追加
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_AtariAdd( SCRATCH_WORK* wk )
{
	int i;

	for( i=0; i < SCRATCH_CARD_ATARI_NUM ;i++ ){

		wk->p_d_atari[i] = ScratchObj_Create(&wk->scratch_clact, SCRATCH_ID_OBJ_D_POKE, 
											ANM_POKE_REE, d_atari_pos[i].x, d_atari_pos[i].y, 
											DISP_MAIN,
											SCRATCH_OBJ_BG_PRI_H, 
											SCRATCH_OBJ_PRI_H );
		ScratchObj_Vanish( wk->p_d_atari[i], SCRATCH_VANISH_ON );		//非表示
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	下画面ボタン追加
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_ButtonAdd( SCRATCH_WORK* wk )
{
	//下：ボタンOBJ追加
	wk->p_d_button = ScratchObj_Create( &wk->scratch_clact, SCRATCH_ID_OBJ_D_BUTTON, 
										ANM_BUTTON_NEXT, d_next_button_pos.x, d_next_button_pos.y, 
										DISP_MAIN,
										SCRATCH_OBJ_BG_PRI_N, 
										SCRATCH_OBJ_PRI_H );
	ScratchObj_Vanish( wk->p_d_button, SCRATCH_VANISH_ON );		//非表示

	//下：真ん中ウィンドウOBJ追加
	wk->p_d_win = ScratchObj_Create(&wk->scratch_clact, SCRATCH_ID_OBJ_D_BUTTON, 
									ANM_CENTER_WIN, d_win_pos.x, d_win_pos.y, 
									DISP_MAIN,
									SCRATCH_OBJ_BG_PRI_N, 
									SCRATCH_OBJ_PRI_H );
	ScratchObj_Vanish( wk->p_d_win, SCRATCH_VANISH_ON );		//非表示

	return;
}

//--------------------------------------------------------------
/**
 * @brief	下画面カード削除
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_CardDelete( SCRATCH_WORK* wk )
{
	int i;

	for( i=0; i < SCRATCH_D_CARD_MAX ;i++ ){
		if( wk->p_d_card[i] != NULL ){
			ScratchObj_Delete( wk->p_d_card[i] );
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 削除
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void ScratchCommon_Delete( SCRATCH_WORK* wk )
{
	int i;

	for( i=0; i < SCRATCH_U_POKE_MAX ;i++ ){
		if( wk->p_u_poke[i] != NULL ){
			ScratchObj_Delete( wk->p_u_poke[i] );
		}
	}

	for( i=0; i < SCRATCH_D_POKE_MAX ;i++ ){
		if( wk->p_d_poke[i] != NULL ){
			ScratchObj_Delete( wk->p_d_poke[i] );
		}
	}

	if( wk->p_d_button != NULL ){
		ScratchObj_Delete( wk->p_d_button );
	}

	if( wk->p_d_win != NULL ){
		ScratchObj_Delete( wk->p_d_win );
	}

	for( i=0; i < SCRATCH_CARD_ATARI_NUM ;i++ ){
		if( wk->p_d_waku[i] != NULL ){
			ScratchObj_Delete( wk->p_d_waku[i] );
		}
	}

	for( i=0; i < SCRATCH_CARD_ATARI_NUM ;i++ ){
		if( wk->p_d_atari[i] != NULL ){
			ScratchObj_Delete( wk->p_d_atari[i] );
		}
	}

	if( wk->p_d_atari_win != NULL ){
		ScratchObj_Delete( wk->p_d_atari_win );
	}

	//ボタンフォント削除
	FontProc_UnloadFont( FONT_BUTTON );
	
	//パレットフェード開放
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_OBJ );
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_BG );

	//パレットフェードシステム開放
	PaletteFadeFree( wk->pfd );
	wk->pfd = NULL;

	ScratchClact_DeleteCellObject(&wk->scratch_clact);		//2Dオブジェクト関連領域開放

	MSG_DsIconFlashDelete( wk->msgicon );
	MSGMAN_Delete( wk->msgman );							//メッセージマネージャ開放
	WORDSET_Delete( wk->wordset );
	STRBUF_Delete( wk->msg_buf );							//メッセージバッファ開放
	STRBUF_Delete( wk->tmp_buf );							//メッセージバッファ開放
	NUMFONT_Delete( wk->num_font );

	sys_FreeMemoryEz( wk->p_char );							//キャラクターデータ
	sys_FreeMemoryEz( wk->buf );							//キャラクターデータをコピーしたバッファ

	ScratchExitBmpWin( wk->bmpwin );						//BMPウィンドウ開放
	Scratch_BgExit( wk->bgl );								//BGL削除
	ArchiveDataHandleClose( wk->hdl );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	復帰
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_Recover( SCRATCH_WORK* wk )
{
	int i;

	Scratch_InitSub1();

	wk->bgl	= GF_BGL_BglIniAlloc( HEAPID_SCRATCH );

	Scratch_InitSub2( wk );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 初期化1
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_InitSub1( void )
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
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_InitSub2( SCRATCH_WORK* wk )
{
	int i,flip;

	wk->hdl= ArchiveDataHandleOpen( ARC_SCRATCH, HEAPID_SCRATCH );
	Scratch_BgInit( wk );								//BG初期化
	Scratch_ObjInit( wk );								//OBJ初期化

	//メッセージデータマネージャー作成
	wk->msgman = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, 
								NARC_msg_scratch_contents_dat, HEAPID_SCRATCH );

	wk->wordset = WORDSET_Create( HEAPID_SCRATCH );
	wk->msg_buf = STRBUF_Create( SCRATCH_MSG_BUF_SIZE, HEAPID_SCRATCH );
	wk->tmp_buf = STRBUF_Create( SCRATCH_MSG_BUF_SIZE, HEAPID_SCRATCH );

	//フォントパレット
	SystemFontPaletteLoad( PALTYPE_MAIN_BG, BS_FONT_PAL * 32, HEAPID_SCRATCH );
	SystemFontPaletteLoad( PALTYPE_SUB_BG, BS_FONT_PAL * 32, HEAPID_SCRATCH );
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, BS_MSGFONT_PAL * 32, HEAPID_SCRATCH );
	TalkFontPaletteLoad( PALTYPE_SUB_BG, BS_MSGFONT_PAL * 32, HEAPID_SCRATCH );

	//ボタンフォントロード
	FontProc_LoadFont( FONT_BUTTON, HEAPID_SCRATCH );

	//8x8フォント作成(使用bmpwin[BMPWIN_TYPE]のパレット(BS_FONT_PAL)を使用
	wk->num_font = NUMFONT_Create( 15, 14, FBMP_COL_NULL, HEAPID_SCRATCH );

	//ビットマップ追加
	ScratchAddBmpWin( wk->bgl, wk->bmpwin );

	//上下逆なので注意
	wk->msgicon = MSG_DsIconFlashAdd( NULL, MSG_DSI_SUBDISP, BS_MSGFONT_PAL, HEAPID_SCRATCH );

	//キャラクターデータをバッファに読込、転送(あとで開放する)
	Scratch_Touch_0( wk );
			
	//バッファを生成(あとで開放する)
	Scratch_Touch_1( wk );

	//キャラクターデータをコピー
	Scratch_Touch_1_5( wk );

	GF_Disp_DispOn();

	//上：ポケモンOBJ追加
	for( i=0; i < SCRATCH_U_POKE_MAX ;i++ ){
		wk->p_u_poke[i] = ScratchObj_Create(&wk->scratch_clact, SCRATCH_ID_OBJ_U_POKE, 
											ANM_POKE_REE + i, 
											SCRATCH_U_POKE_X,
											SCRATCH_U_POKE_Y + (i * SCRATCH_U_POKE_WIDTH_Y),
											DISP_SUB,
											SCRATCH_OBJ_BG_PRI_H, 
											SCRATCH_OBJ_PRI_H );
		ScratchObj_Vanish( wk->p_u_poke[i], SCRATCH_VANISH_ON );		//非表示
	}

	//下：ポケモンOBJ追加
	for( i=0; i < SCRATCH_D_POKE_MAX ;i++ ){
		wk->p_d_poke[i] = ScratchObj_Create(&wk->scratch_clact, SCRATCH_ID_OBJ_D_POKE, 
											ANM_POKE_REE, d_poke_pos[i].x, d_poke_pos[i].y, 
											DISP_MAIN,
											SCRATCH_OBJ_BG_PRI_L, 
											SCRATCH_OBJ_PRI_N );
		ScratchObj_Vanish( wk->p_d_poke[i], SCRATCH_VANISH_ON );		//非表示
	}

	Scratch_ButtonAdd( wk );					//下：ボタンOBJ追加

	//下：カードOBJ追加
	for( i=0; i < SCRATCH_D_CARD_MAX ;i++ ){
		wk->p_d_card[i] = ScratchObj_Create(&wk->scratch_clact, SCRATCH_ID_OBJ_D_CARD, 
											(ANM_CARD_BLUE + i), d_card_pos[i].x, d_card_pos[i].y, 
											DISP_MAIN,
											//SCRATCH_OBJ_BG_PRI_H, 
											SCRATCH_OBJ_BG_PRI_N, 
											SCRATCH_OBJ_PRI_H );
		//ScratchObj_Vanish( wk->p_d_card[i], SCRATCH_VANISH_ON );		//非表示
		
		//画面外にしておく
		ScratchObj_SetObjPos(	wk->p_d_card[i], (d_card_pos[i].x + SCRATCH_CARD_IN_OFFSET_X), 
								d_card_pos[i].y );

		ScratchObj_Priority( wk->p_d_card[i], (SCRATCH_OBJ_PRI_L+i) );
	}

	//当たりウィンドウ
	wk->p_d_atari_win = ScratchObj_Create(	&wk->scratch_clact, SCRATCH_ID_OBJ_D_ATARI, 
											0, 
											d_atari_win_pos.x,
											d_atari_win_pos.y,
											DISP_MAIN,
											SCRATCH_OBJ_BG_PRI_H, 
											SCRATCH_OBJ_PRI_N );
	ScratchObj_Vanish( wk->p_d_atari_win, SCRATCH_VANISH_ON );		//非表示

#if 1
	//VRAM割り当てがC=128,I=16しかなくて、Cは通信用で使用不可なので。

	//画面出力を上下入れ替える
	GX_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );
#endif

	InitTPSystem();										// タッチパネルシステム初期化
	InitTPNoBuff(1);
	//InitTPNoBuff(2);
	//InitTPNoBuff(3);
	//InitTPNoBuff(4);

	sys_VBlankFuncChange( VBlankFunc, (void*)wk );		//VBlankセット
	return;
}

//--------------------------------------------------------------
/**
 * @brief	下画面「はい、いいえ」タッチシステム確保
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_AllocTouchSub( SCRATCH_WORK* wk )
{
	wk->touch_subwin = TOUCH_SW_AllocWork( HEAPID_SCRATCH );	//確保
	return;
}

//--------------------------------------------------------------
/**
 * @brief	下画面「はい、いいえ」タッチシステム初期化
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_InitTouchSub( SCRATCH_WORK* wk )
{
	TOUCH_SW_PARAM cp_param;

	cp_param.p_bgl		= wk->bgl;				//BGLシステムワーク
	cp_param.bg_frame	= SCRATCH_FRAME_D_MSG;	//BGナンバー
	//cp_param.char_offs	= (1024 - 64);			//キャラクタ転送オフセット((ｷｬﾗｸﾀ単位 1/32byte)
	cp_param.char_offs	= (1024 - 128);			//キャラクタ転送オフセット((ｷｬﾗｸﾀ単位 1/32byte)
	cp_param.pltt_offs	= SCRATCH_TOUCH_SUB_PAL;//パレット転送オフセット(ﾊﾟﾚｯﾄ1本分単位 1/32byte)
	cp_param.x			= 24;					//x座標(ｷｬﾗｸﾀ単位)
	cp_param.y			= 8;					//y座標(ｷｬﾗｸﾀ単位)

	TOUCH_SW_Init( wk->touch_subwin, &cp_param );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	下画面「はい、いいえ」タッチシステム削除
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_FreeTouchSub( SCRATCH_WORK* wk )
{
	TOUCH_SW_FreeWork( wk->touch_subwin );
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
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_BgInit( SCRATCH_WORK* wk )
{
	SetVramBank();
	SetBgHeader( wk->bgl );

	//パレットフェードシステムワーク作成
	wk->pfd = PaletteFadeInit( HEAPID_SCRATCH );

	//リクエストデータをmallocしてセット
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_OBJ, FADE_PAL_ALL_SIZE, HEAPID_SCRATCH );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_BG, FADE_PAL_ALL_SIZE, HEAPID_SCRATCH );

	//上画面背景
	Scratch_SetMainBgGraphic( wk, SCRATCH_FRAME_U_BG );
	Scratch_SetMainBgPalette();

	//上画面役
	Scratch_SetYakuBgGraphic( wk, SCRATCH_FRAME_U_YAKU );

	//下画面バック
	//Scratch_SetBackBgGraphic( wk, SCRATCH_FRAME_D_MSG );

	//下画面背景、パレットセット
	Scratch_SetSubBgGraphic( wk, SCRATCH_FRAME_D_SUB );
	Scratch_SetSubBgPalette();

	//下画面タッチ、パレットセット
	Scratch_SetTouchBgGraphic( wk, SCRATCH_FRAME_D_TOUCH );

	//下画面当たりウィンドウ
	//Scratch_SetAtariBgGraphic( wk, SCRATCH_FRAME_D_ATARI );

	GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_MSG, VISIBLE_OFF );		//下メッセージオフ
	GF_Disp_GXS_VisibleControl( SCRATCH_U_MASK_MSG, VISIBLE_OFF );		//上メッセージオフ
	return;
}

//--------------------------------------------------------------
/**
 * @brief	OBJ関連初期化
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_ObjInit( SCRATCH_WORK* wk )
{
	ScratchClact_InitCellActor(	&wk->scratch_clact );
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
static void Scratch_BgExit( GF_BGL_INI * ini )
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
	GF_BGL_BGControlExit( ini, SCRATCH_FRAME_U_BG );
	GF_BGL_BGControlExit( ini, SCRATCH_FRAME_U_YAKU );
	GF_BGL_BGControlExit( ini, SCRATCH_FRAME_U_MSG );
	GF_BGL_BGControlExit( ini, SCRATCH_FRAME_D_SUB );
	GF_BGL_BGControlExit( ini, SCRATCH_FRAME_D_TOUCH );
	GF_BGL_BGControlExit( ini, SCRATCH_FRAME_D_MSG );
	//GF_BGL_BGControlExit( ini, SCRATCH_FRAME_D_ATARI );

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
	SCRATCH_WORK* wk = work;

	//ポケモンステータス表示中
	if( wk->child_proc != NULL ){
		return;
	}

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
		GX_VRAM_BG_128_A,				//メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,			//メイン2DエンジンのBG拡張パレット

		GX_VRAM_SUB_BG_128_C,			//サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		//サブ2DエンジンのBG拡張パレット

		GX_VRAM_OBJ_128_B,				//メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		//メイン2DエンジンのOBJ拡張パレット

		GX_VRAM_SUB_OBJ_16_I,			//サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	//サブ2DエンジンのOBJ拡張パレット

		GX_VRAM_TEX_NONE,				//テクスチャイメージスロット
		GX_VRAM_TEXPLTT_NONE			//テクスチャパレットスロット
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

	//上画面128k=0x20000内に収める
	//--------------------------------------------------------------------------------
	{	//BG(フォント)(会話、メニュー)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x1000, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, SCRATCH_FRAME_U_MSG, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( SCRATCH_FRAME_U_MSG, 32, 0, HEAPID_SCRATCH );
		GF_BGL_ScrClear( ini, SCRATCH_FRAME_U_MSG );
	}

	//--------------------------------------------------------------------------------
	{	//上画面(背景)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x1000, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			3, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, SCRATCH_FRAME_U_BG, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, SCRATCH_FRAME_U_BG );
	}

	//--------------------------------------------------------------------------------
	{	//上画面(役)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, SCRATCH_FRAME_U_YAKU, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, SCRATCH_FRAME_U_YAKU );
	}

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//下画面128k=0x20000内に収める
	//--------------------------------------------------------------------------------
	{	//下画面(背景)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_16,
			//GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, SCRATCH_FRAME_D_SUB, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, SCRATCH_FRAME_D_SUB );
	}

	//--------------------------------------------------------------------------------
	{	//下画面(タッチ)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, SCRATCH_FRAME_D_TOUCH, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, SCRATCH_FRAME_D_TOUCH );
	}

	//--------------------------------------------------------------------------------
	{	//下画面(メッセージ)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, SCRATCH_FRAME_D_MSG, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( SCRATCH_FRAME_D_MSG, 32, 0, HEAPID_SCRATCH );
		GF_BGL_ScrClear( ini, SCRATCH_FRAME_D_MSG );
	}

	//--------------------------------------------------------------------------------
#if 0
	{	//下画面(当たりウィンドウ)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, SCRATCH_FRAME_D_ATARI, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( SCRATCH_FRAME_D_ATARI, 32, 0, HEAPID_SCRATCH );
		GF_BGL_ScrClear( ini, SCRATCH_FRAME_D_ATARI );
	}
#endif

	//G2_SetBG0Priority( 0 );

	GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_TOUCH, VISIBLE_OFF );			//削り面オフ
	GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_SUB, VISIBLE_OFF );			//カード面オフ
	//GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_ATARI, VISIBLE_OFF );			//当たりウィンオフ
	GF_Disp_GXS_VisibleControl( SCRATCH_U_MASK_YAKU, VISIBLE_OFF );			//役オフ
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
 * @param	wk		SCRATCH_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_SetMainBgGraphic( SCRATCH_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, NARC_scratch_u_bg00_NCGR, 
							wk->bgl, frm, 0, 0, 0, HEAPID_SCRATCH );

	ArcUtil_HDL_ScrnSet(wk->hdl, NARC_scratch_u_bg00_NSCR, 
						wk->bgl, frm, 0, 0, 0, HEAPID_SCRATCH );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面背景(役)
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_SetYakuBgGraphic( SCRATCH_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, NARC_scratch_u_bg01_NCGR, 
							wk->bgl, frm, 0, 0, 0, HEAPID_SCRATCH );

	ArcUtil_HDL_ScrnSet(wk->hdl, NARC_scratch_u_bg01_NSCR, 
						wk->bgl, frm, 0, 0, 0, HEAPID_SCRATCH );
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
static void Scratch_SetMainBgPalette( void )
{
	void *buf;
	NNSG2dPaletteData *dat;

	buf = ArcUtil_PalDataGet( ARC_SCRATCH, NARC_scratch_u_bg_NCLR, &dat, HEAPID_SCRATCH );

	DC_FlushRange( dat->pRawData, (sizeof(u16)*16*5) );
	//GX_LoadBGPltt( dat->pRawData, 0, (sizeof(u16)*16*5) );		//メイン
	GXS_LoadBGPltt( dat->pRawData, 0, (sizeof(u16)*16*5) );		//サブ

	sys_FreeMemoryEz(buf);
	return;
}

//--------------------------------------------------------------
/**
 * @brief	下画面背景パレット設定
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_SetSubBgPalette( void )
{
	void *buf;
	NNSG2dPaletteData *dat;

	buf = ArcUtil_PalDataGet( ARC_SCRATCH, NARC_scratch_l_bg_NCLR, &dat, HEAPID_SCRATCH );

	DC_FlushRange( dat->pRawData, (sizeof(u16)*16*9) );
	//GXS_LoadBGPltt( dat->pRawData, 0, (sizeof(u16)*16*9) );		//サブ
	//GX_LoadBGPltt( dat->pRawData, 0, (sizeof(u16)*16*9) );		//メイン
	GX_LoadBGPltt( dat->pRawData, 0, (sizeof(u16)*16*10) );		//メイン

	sys_FreeMemoryEz(buf);
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：下画面バック
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_SetBackBgGraphic( SCRATCH_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, NARC_scratch_l_bg00_NCGR, 
							wk->bgl, frm, 0, 0, 0, HEAPID_SCRATCH );

	ArcUtil_HDL_ScrnSet(wk->hdl, NARC_scratch_l_bg00_NSCR, 
						wk->bgl, frm, 0, 0, 0, HEAPID_SCRATCH );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：下画面背景
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_SetSubBgGraphic( SCRATCH_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, NARC_scratch_l_bg01_NCGR, 
							wk->bgl, frm, 0, 0, 0, HEAPID_SCRATCH );

	ArcUtil_HDL_ScrnSet(wk->hdl, NARC_scratch_l_bg01_NSCR, 
						wk->bgl, frm, 0, 0, 0, HEAPID_SCRATCH );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：下画面削り面
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_SetTouchBgGraphic( SCRATCH_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, NARC_scratch_l_bg01gin_NCGR, 
	//ArcUtil_HDL_BgCharSet(	wk->hdl, NARC_scratch_l_bg00_NCGR, 
	//ArcUtil_HDL_BgCharSet(	wk->hdl, NARC_scratch_l_bg01_NCGR, 
							wk->bgl, frm, 0, 0, 0, HEAPID_SCRATCH );

	ArcUtil_HDL_ScrnSet(wk->hdl, NARC_scratch_l_bg01gin_NSCR, 
	//ArcUtil_HDL_ScrnSet(wk->hdl, NARC_scratch_l_bg00_NSCR, 
	//ArcUtil_HDL_ScrnSet(wk->hdl, NARC_scratch_l_bg01_NSCR, 
						wk->bgl, frm, 0, 0, 0, HEAPID_SCRATCH );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：下画面当たりウィンドウ
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
#if 0
static void Scratch_SetAtariBgGraphic( SCRATCH_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, NARC_scratch_l_bg01win_NCGR, 
							wk->bgl, frm, 0, 0, 0, HEAPID_SCRATCH );

	ArcUtil_HDL_ScrnSet(wk->hdl, NARC_scratch_l_bg01win_NSCR, 
						wk->bgl, frm, 0, 0, 0, HEAPID_SCRATCH );
	return;
}
#endif


//==============================================================================================
//
//	メッセージ関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	メッセージ表示
 *
 * @param	wk		SCRATCH_WORK型のポインタ
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
static u8 ScratchWriteMsg( SCRATCH_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
	GF_BGL_BmpWinDataFill( win, b_col );			//塗りつぶし
	MSGMAN_GetString( wk->msgman, msg_id, wk->tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( wk->wordset, wk->msg_buf, wk->tmp_buf );

	return GF_STR_PrintColor( win, font, wk->msg_buf, x, y, wait, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );
}

// NONMATCHING
asm static u8 ScratchWriteMsg_ov111_21D2424( SCRATCH_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x10
	add r4, r1, #0
	add r1, sp, #0x38
	ldrb r1, [r1]
	add r5, r0, #0
	add r0, r4, #0
	add r7, r2, #0
	add r6, r3, #0
	bl GF_BGL_BmpWinDataFill
	ldr r0, [r5, #0x38]
	ldr r2, [r5, #0x44]
	add r1, r7, #0
	bl MSGMAN_GetString
	ldr r0, [r5, #0x3c]
	ldr r1, [r5, #0x40]
	ldr r2, [r5, #0x44]
	bl WORDSET_ExpandStr
	add r0, sp, #0x3c
	ldrb r0, [r0]
	ldr r1, [r5, #0x40]
	mov r2, #0
	bl FontProc_GetPrintStrWidth
	add r0, r0, #1
	lsr r0, r0, #1
	sub r3, r6, r0
	ldr r0, [sp, #0x28]
	add r2, sp, #0x18
	str r0, [sp]
	ldr r0, [sp, #0x2c]
	str r0, [sp, #4]
	add r0, sp, #0x38
	ldrb r1, [r0]
	ldrb r0, [r2, #0x18]
	ldrb r2, [r2, #0x1c]
	lsl r0, r0, #0x18
	lsl r2, r2, #0x18
	lsr r0, r0, #8
	lsr r2, r2, #0x10
	orr r0, r2
	orr r0, r1
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	add r1, sp, #0x3c
	ldrb r1, [r1]
	ldr r2, [r5, #0x40]
	add r0, r4, #0
	bl GF_STR_PrintColor
	add sp, #0x10
	pop {r3, r4, r5, r6, r7, pc}
}

//--------------------------------------------------------------
/**
 * @brief	メッセージ表示(塗りつぶしなし)
 *
 * @param	wk		SCRATCH_WORK型のポインタ
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
static u8 ScratchWriteMsgSimple( SCRATCH_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
	MSGMAN_GetString( wk->msgman, msg_id, wk->tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( wk->wordset, wk->msg_buf, wk->tmp_buf );

	return GF_STR_PrintColor( win, font, wk->msg_buf, x, y, wait, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );
}

//--------------------------------------------------------------
/**
 * @brief	確認メッセージ表示
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
static u8 Scratch_KakuninMsg( SCRATCH_WORK* wk )
{
	u8 msg_index;

	ScratchWriteMenuWin( wk->bgl, &wk->bmpwin[BMPWIN_TALK] );
	msg_index = ScratchWriteMsg(wk, &wk->bmpwin[BMPWIN_TALK], msg_scratch_contents_07, 
								1, 1, MSG_ALLPUT, 
								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, BS_FONT );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_TALK] );
	return msg_index;
}

//--------------------------------------------------------------
/**
 * @brief	○まいめ表示
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
#if 0
static u8 Scratch_CardNumMsg( SCRATCH_WORK* wk )
{
	u8 msg_index;

	//○まいめをセット
	Scratch_SetNumber( wk, 0, (wk->card_num+1) );

	msg_index = ScratchWriteMsg(wk, &wk->bmpwin[BMPWIN_START], msg_scratch_contents_01, 
								8*4, 1+4, MSG_ALLPUT, 
								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BS_FONT );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_START] );
	return msg_index;
}
#endif

//--------------------------------------------------------------
/**
 * @brief	○まいめの　カードを　えらんでください
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
static u8 Scratch_CardSelectMsg( SCRATCH_WORK* wk )
{
	u8 msg_index;

	//○まいめをセット
	Scratch_SetNumber( wk, 0, (wk->card_num+1) );

	msg_index = ScratchWriteMsg(wk, &wk->bmpwin[BMPWIN_SELECT], msg_scratch_contents_13, 
								1, 1, MSG_ALLPUT, 
								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, FONT_TALK );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_SELECT] );
	return msg_index;
}

//--------------------------------------------------------------
/**
 * @brief	スクラッチスタート表示
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
static u8 Scratch_StartMsg( SCRATCH_WORK* wk )
{
	u8 msg_index;

	//○まいめをセット
	Scratch_SetNumber( wk, 0, (wk->card_num+1) );

    // MatchComment: ScratchWriteMsg -> ScratchWriteMsg_ov111_21D2424, change 3rd (0-indexed) arg from 8*1-1 -> 8*6
	msg_index = ScratchWriteMsg_ov111_21D2424(wk, &wk->bmpwin[BMPWIN_START], msg_scratch_contents_10, 
								8*6, 1+4, MSG_ALLPUT, 
								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BS_FONT );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_START] );
	return msg_index;
}

//--------------------------------------------------------------
/**
 * @brief	３つそろった！表示
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
static u8 Scratch_AtariMsg( SCRATCH_WORK* wk )
{
	u8 msg_index,i;

	msg_index = 0;

	//削ったポケモンに変更して表示
	for( i=0; i < SCRATCH_CARD_ATARI_NUM ;i++ ){
		ScratchObj_AnmChg( wk->p_d_atari[i], wk->card_poke[ wk->kezuri_card[i] ] );
		ScratchObj_Vanish( wk->p_d_atari[i], SCRATCH_VANISH_OFF );		//表示
	}

	ScratchObj_Vanish( wk->p_d_atari_win, SCRATCH_VANISH_OFF );			//表示
	ScratchObj_SetAnmFlag( wk->p_d_atari_win, 1 );						//オートアニメ
	//GF_Disp_GX_VisibleControl( SCRATCH_D_MASK_ATARI, VISIBLE_ON );		//下当たりウィンドウオン

	Snd_SePlay( SE_D_CARD_ATARI );

	/*当たりメッセージはいるのでは？*/
	//msg_index = ScratchWriteMsg(wk, &wk->bmpwin[BMPWIN_ATARI], msg_scratch_contents_11, 
	//							8*2, 1+4, MSG_ALLPUT, 
	//							FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BS_FONT );

	//GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_ATARI] );
	return msg_index;
}

//--------------------------------------------------------------
/**
 * @brief	ざんねん表示
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
static u8 Scratch_HazureMsg( SCRATCH_WORK* wk )
{
	u8 msg_index;

	Snd_SePlay( SE_D_CARD_HAZURE );
    // MatchComment: ScratchWriteMsg -> ScratchWriteMsg_ov111_21D2424, third (0-indexed) arg from 8*3-1 -> 8*6
	msg_index = ScratchWriteMsg_ov111_21D2424(wk, &wk->bmpwin[BMPWIN_HAZURE], msg_scratch_contents_12, 
								8*6, 1+4, MSG_ALLPUT, 
								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BS_FONT );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_HAZURE] );
	return msg_index;
}

//--------------------------------------------------------------
/**
 * @brief	つぎのカードへ表示
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
static u8 Scratch_NextMsg( SCRATCH_WORK* wk )
{
	u8 msg_index;

    // MatchComment: ScratchWriteMsg -> ScratchWriteMsg_ov111_21D2424, third (0-indexed) arg from 4 -> 8*6
	msg_index = ScratchWriteMsg_ov111_21D2424(wk, &wk->bmpwin[BMPWIN_NEXT], msg_scratch_contents_05, 
								8*6, 1, MSG_ALLPUT, 
								//FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_BUTTON );
								FBMP_COL_BLACK,FBMP_COL_BLACK,FBMP_COL_NULL, FONT_BUTTON );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_NEXT] );
	return msg_index;
}

//--------------------------------------------------------------
/**
 * @brief	やめる表示
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
static u8 Scratch_YameruMsg( SCRATCH_WORK* wk )
{
	u8 msg_index;

    // MatchComment: ScratchWriteMsg -> ScratchWriteMsg_ov111_21D2424, third (0-indexed) arg from 4 -> 8*3
	msg_index = ScratchWriteMsg_ov111_21D2424(wk, &wk->bmpwin[BMPWIN_YAMERU], msg_scratch_contents_06, 
								8*3, 1, MSG_ALLPUT, 
								//FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_BUTTON );
								FBMP_COL_BLACK,FBMP_COL_BLACK,FBMP_COL_NULL, FONT_BUTTON );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_YAMERU] );
	return msg_index;
}

//--------------------------------------------------------------
/**
 * @brief	役表示
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 * @param	no		何番目の役か
 * @param	item	アイテムナンバー
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
//static const u8 yaku_offset_y[SCRATCH_ATARI_MAX] = { 0, 4, 0, 4 };		//表示オフセット
static const u8 yaku_offset_y[SCRATCH_ATARI_MAX] = { 10, 12, 6, 8 };		//表示オフセット
#define SCRATCH_ITEM_NUM_X	(8*3)

static u8 Scratch_YakuMsg( SCRATCH_WORK* wk, u8 no )
{
	u8 msg_index;

	//アイテム名
	WORDSET_RegisterItemName( wk->wordset, 0, wk->atari_item[no] );
    // MatchComment: ScratchWriteMsg -> ScratchWriteMsg_ov111_21D2424, third (0-indexed) arg from 4 -> 8*6-4
	msg_index = ScratchWriteMsg_ov111_21D2424(wk, &wk->bmpwin[BMPWIN_ITEM1+no], msg_scratch_contents_02, 
								8*6-4, yaku_offset_y[no], MSG_ALLPUT, 
								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BS_FONT );

	//アイテム数
	//msg_index = ScratchWriteMsgSimple(wk, &wk->bmpwin[BMPWIN_ITEM1+no], msg_scratch_contents_03, 
	//							SCRATCH_ITEM_NUM_X, 16+yaku_offset_y[no], MSG_ALLPUT, 
	//							FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, BS_FONT );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_ITEM1+no] );
	return msg_index;
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
 * @param	wk		SCRATCH_WORK型のポインタ
 * @param	bufID	バッファID
 * @param	number	セットする数値
 *
 * @retval	none
 *
 * 桁を固定にしている
 */
//--------------------------------------------------------------
static void Scratch_SetNumber( SCRATCH_WORK* wk, u32 bufID, s32 number )
{
	WORDSET_RegisterNumber( wk->wordset, bufID, number, 1, 
							NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン名をセット
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 * @param	bufID	バッファID
 * @param	ppp		POKEMON_PASO_PARAM
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Scratch_SetPokeName( SCRATCH_WORK* wk, u32 bufID, POKEMON_PASO_PARAM* ppp )
{
	WORDSET_RegisterPokeMonsName( wk->wordset, bufID, ppp );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	プレイヤー名をセット
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 * @param	bufID	バッファID
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Scratch_SetPlayerName( SCRATCH_WORK* wk, u32 bufID )
{
	WORDSET_RegisterPlayerName( wk->wordset, bufID, SaveData_GetMyStatus(wk->sv) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	プレイヤー名を表示
 *
 * @param	wk		SCRATCH_WORK型のポインタ
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
static void PlayerNameWrite( SCRATCH_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font )
{
	u32 col;
	const MYSTATUS* my;
	STRBUF* player_buf;								//プレイヤー名バッファポインタ

	my = SaveData_GetMyStatus( wk->sv );
	player_buf = STRBUF_Create( PLAYER_NAME_BUF_SIZE, HEAPID_SCRATCH );
	
	GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );			//塗りつぶし

	STRBUF_SetStringCode( player_buf, MyStatus_GetMyName(my) );

	if( MyStatus_GetMySex(my) == PM_MALE ){
		col = COL_BLUE;
	}else{
		col = COL_RED;
	}

	GF_STR_PrintColor( win, font, player_buf, x, y, MSG_ALLPUT, col, NULL );

	STRBUF_Delete( player_buf );					//メッセージバッファ開放

	GF_BGL_BmpWinOn( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン名を表示(性別指定)
 *
 * @param	wk		SCRATCH_WORK型のポインタ
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
static void PokeNameWriteEx( SCRATCH_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font, u16 monsno, u8 sex )
{
	u8 x_pos;
	u32 msg_id,col;
	MSGDATA_MANAGER* man;
	STRBUF* buf;
	POKEMON_PARAM* poke;
	STRCODE sel_poke_buf[POKE_NAME_BUF_SIZE];					//ポケモン名バッファポインタ

	GF_BGL_BmpWinDataFill( win, b_col );						//塗りつぶし

	//ポケモン名を取得
	man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_itemname_dat, HEAPID_SCRATCH );
	buf = MSGMAN_AllocString( man, monsno );
	MSGMAN_Delete( man );

	GF_STR_PrintColor( win, font, buf, x, y, MSG_ALLPUT, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );

	STRBUF_Delete( buf );

	GF_BGL_BmpWinOn( win );
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
 * @param	wk		SCRATCH_WORK型のポインタ
 * @param	seq		シーケンスのポインタ
 * @param	next	次のシーケンス定義
 *
 * @return	none
 */
//--------------------------------------------------------------
static void NextSeq( SCRATCH_WORK* wk, int* seq, int next )
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
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void BgCheck( SCRATCH_WORK* wk )
{
	if( sys.cont & PAD_KEY_UP ){
		if( sys.cont & PAD_BUTTON_L ){
			//GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_R ){
			//GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
		}
	}else if( sys.cont & PAD_KEY_DOWN ){
		if( sys.cont & PAD_BUTTON_A ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_B ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
		}else if( sys.cont & PAD_BUTTON_L ){
		}else if( sys.cont & PAD_BUTTON_R ){
		}
	}else if( sys.cont & PAD_KEY_LEFT ){
		if( sys.cont & PAD_BUTTON_L ){
			//GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_R ){
			//GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
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
 * @brief	カーソルＸ取得
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"X"
 */
//--------------------------------------------------------------
static u16 GetCsrX( SCRATCH_WORK* wk )
{
	return wk->csr_pos;
}

//ランダムがうまくいかなかった時の配置★
#if 0
static const u8 atari_poke_tbl[SCRATCH_D_POKE_MAX][SCRATCH_EASY_ATARI_NUM] = {
#if 0
	//4はセンターなので必ず入れる
	{ 1,2,3,4,7 },
	{ 2,3,4,5,6 },
	{ 1,2,4,5,7 },
	{ 1,2,4,7,6 },
	{ 1,3,4,5,8 },
	{ 0,1,2,4,7 },
	{ 2,4,5,6,8 },
	{ 0,1,2,4,7 },
	{ 3,4,5,6,8 },
#else
	//4はセンターなので必ず入れる
	{ 1,2,3,4 },
	{ 2,3,4,5 },
	{ 1,2,4,5 },
	{ 1,2,4,7 },
	{ 1,3,4,5 },
	{ 0,1,2,4 },
	{ 2,4,5,6 },
	{ 0,1,2,4 },
	{ 3,4,5,6 },
#endif
};
#endif

//--------------------------------------------------------------
/**
 * @brief	メタモンを配置
 *
 * @param	wk			SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_MetamonPokeSet( SCRATCH_WORK* wk )
{
	int i;
	u8 no;

	//当たりに必要な揃えるポケモンの数を配置
	for( i=0; i < 2 ;i++ ){

		while( 1 ){
			//0-8を取得
			no = ( gf_rand() % SCRATCH_D_POKE_MAX );

			//空いてるカードの場所にメタモンを入れる
			if( wk->card_poke[no] == DUMMY_CARD_POKE ){
				wk->card_poke[no] = ANM_POKE_META;
				break;
			}
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	はずれポケモンを配置
 *
 * @param	wk			SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_OthersPokeSet( SCRATCH_WORK* wk )
{
	int i,j,set_pos,loop_count,no;
	u8 set_poke;

	set_poke	= ( gf_rand() % 4 );	//セットするポケモンナンバー(0-3を決める。メタモン以外)
	loop_count	= 0;

	for( i=0; i < SCRATCH_D_POKE_MAX; i++ ){

		while( 1 ){
			no = ( gf_rand() % SCRATCH_D_POKE_MAX );

			//空いてる場所だったら
			if( wk->card_poke[no] == DUMMY_CARD_POKE ){
				loop_count = 0;
				wk->card_poke[no] = set_poke;

				//3,2,2匹ずつ配置する
				if( (i == 2) || (i == 4) || (i == 6) ){
					set_poke++;										//次のポケモンナンバーにする
					if( set_poke == (SCRATCH_CARD_POKE_MAX-1) ){	//メタモン抜かす
						set_poke = 0;
					}
				}
				break;
			}

			/////////////////////////////////////////////////////////////////////////////////////
			//ランダムがうまくいかなかった時
			loop_count++;
			if( loop_count >= 30 ){
				loop_count = 0;

				//先頭から空いている所に配置
				for( set_pos=0; set_pos < SCRATCH_D_POKE_MAX; set_pos++ ){

					//空いてる場所をサーチ
					if( wk->card_poke[set_pos] == DUMMY_CARD_POKE ){
						wk->card_poke[set_pos] = set_poke;

						//3,2,2匹ずつ配置する
						if( (i == 2) || (i == 4) || (i == 6) ){
							set_poke++;								//次のポケモンナンバーにする
							if( set_poke == (SCRATCH_CARD_POKE_MAX-1) ){	//メタモン抜かす
								set_poke = 0;
							}
						}
						break;		//for抜け]]]]]]]]]]]]]]]]]]]]
					}
				}
				break;				//while抜け]]]]]]]]]]]]]]]]]]
			}
		}
	}

	OS_Printf( "配置したポケモンナンバー\n" );
	for( i=0; i < SCRATCH_D_POKE_MAX; i++ ){
		OS_Printf( "wk->card_poke[%d] = %d\n", i, wk->card_poke[i] );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモンを配置(全種類セット)
 *
 * @param	wk			SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_PokeSet( SCRATCH_WORK* wk )
{
	int i,j,set_pos,loop_count,no;
	u8 set_poke,set_poke_num;

	set_poke	= (SCRATCH_CARD_POKE_MAX-1);	//セットするポケモンナンバー(メタモンからスタート)
	set_poke_num= 0;							//セットしたポケモンの数
	loop_count	= 0;

	for( i=0; i < SCRATCH_D_POKE_MAX; i++ ){

		while( 1 ){
			no = ( gf_rand() % SCRATCH_D_POKE_MAX );

			//空いてる場所だったら
			if( wk->card_poke[no] == DUMMY_CARD_POKE ){
				loop_count = 0;

				wk->card_poke[no] = set_poke;
				set_poke_num++;

				//当たりが揃う分になる前にセットするポケモンを変更する★
				if( set_poke_num == (SCRATCH_CARD_ATARI_NUM-1) ){
					set_poke_num = 0;
					set_poke++;										//次のポケモンナンバーにする
					if( set_poke == SCRATCH_CARD_POKE_MAX ){
						set_poke = 0;
					}
				}
				break;
			}

			/////////////////////////////////////////////////////////////////////////////////////
			//ランダムがうまくいかなかった時
			loop_count++;
			if( loop_count >= 30 ){
				loop_count = 0;

				//先頭から空いている所に配置
				for( set_pos=0; set_pos < SCRATCH_D_POKE_MAX; set_pos++ ){

					//空いてる場所をサーチ
					if( wk->card_poke[set_pos] == DUMMY_CARD_POKE ){

						wk->card_poke[set_pos] = set_poke;
						set_poke_num++;

						//当たりが揃う分になる前にセットするポケモンを変更する
						if( set_poke_num == (SCRATCH_CARD_ATARI_NUM-1) ){
							set_poke_num = 0;
							set_poke++;								//次のポケモンナンバーにする
							if( set_poke == SCRATCH_CARD_POKE_MAX ){
								set_poke = 0;
							}
						}
						break;		//for抜け]]]]]]]]]]]]]]]]]]]]
					}
				}
				break;				//while抜け]]]]]]]]]]]]]]]]]]
			}
		}
	}

	OS_Printf( "配置したポケモンナンバー\n" );
	for( i=0; i < SCRATCH_D_POKE_MAX; i++ ){
		OS_Printf( "wk->card_poke[%d] = %d\n", i, wk->card_poke[i] );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	配置ポケモンを決定
 *
 * @param	wk			SCRATCH_WORK型のポインタ
 * @param	mode		イージ・ハードどちらか
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_CardPokeSet( SCRATCH_WORK* wk, u8 mode )
{
#if 1
	//メタモンポケモンをセット
	Scratch_MetamonPokeSet( wk );

	//その他ポケモンをセット
	Scratch_OthersPokeSet( wk );
#else
	//ポケモンをセット
	Scratch_PokeSet( wk );
#endif
	return;
}

//--------------------------------------------------------------
/**
 * @brief	カードの中のポケモンナンバーをクリア
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_CardPokeClear( SCRATCH_WORK* wk )
{
	int i;

	for( i=0; i < SCRATCH_D_POKE_MAX ;i++ ){
		wk->card_poke[i] = DUMMY_CARD_POKE;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	役をを決定
 *
 * @param	wk			SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_YakuSet( SCRATCH_WORK* wk, u8 mode )
{
	int i,j;
	u16 item_pos[SCRATCH_ATARI_MAX];
	u16 kin_pos;

	//クリア
	for( i=0; i < SCRATCH_ATARI_MAX ;i++ ){
		item_pos[i] = 0xff;
	}

	//「きんのたま」の配置場所
	kin_pos = ( gf_rand() % SCRATCH_ATARI_MAX );

	for( i=0; i < SCRATCH_ATARI_MAX ;i++ ){

		if( i == kin_pos ){
			wk->atari_item[i] = ITEM_KINNOTAMA;			//1つは必ず配置する
		}else{

			while( 1 ){
				item_pos[i] = ( gf_rand() % SCRATCH_EASY_ITEM );

				//同じアイテムを配置しないようにする
				for( j=0; j < i ;j++ ){

					//すでに同じアイテムがあったら
					if( item_pos[j] == item_pos[i] ){
						break;
					}
				}

				//同じアイテムがなかったら
				if( j == i ){
					wk->atari_item[i] = scratch_easy_item[ item_pos[i] ];
					break;		//while抜け
				}
			}
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	カードを中心に移動
 *
 * @param	none
 *
 * @return	"FALSE = 継続中、TRUE = 目標到達"
 */
//--------------------------------------------------------------
//static const int card_x_spd_tbl[SCRATCH_D_CARD_MAX] = { 4, 1, -1, -4 };
static const int card_x_spd_tbl[SCRATCH_D_CARD_MAX] = { 8, 2, -2, -8 };
//static const int card_y_spd_tbl[SCRATCH_D_CARD_MAX] = { -2, -2, -2, -2 };
static const int card_y_spd_tbl[SCRATCH_D_CARD_MAX] = { -4, -4, -4, -4 };

#define CARD_MOVE_NUM	(8)	//この回数で中心にもっていく
static const s8 card_move_x_tbl[SCRATCH_D_CARD_MAX][CARD_MOVE_NUM] = {
	{ 9,	9,	9,	8,	7,	6,	5,	4, },	//

	//{ 4,	4,	3,	3,	2,	2,	1,	1, },	//
	{ 2,	2,	2,	1,	1,	1,	1,	1, },	//

	//{ -4,	-4,	-3, -3, -2, -2,	-1,	-1, },	//3つ目正面に出る
	{ -8,	-8,-8,-7, -7, -6, -5, -4, },

	//{ -9,	-9,	-9,	-8,	-7,	-6,	-5,	-4, },	//
	{ -18,	-16,-15,-14,-13,-12,-10,-8, },	//
};
static const s8 card_move_y_tbl[CARD_MOVE_NUM] = { -4,	-4,	-4,	-4,	-4,	-4,	-4,	-4, };	//38

static BOOL Scratch_CardCenterMove( SCRATCH_WORK* wk, u8 pos )
{
	int now_x,now_y,set_x,set_y;

	//目標回数移動した
	if( wk->move_num >= CARD_MOVE_NUM ){
		return TRUE;
	}

	//現在の位置を取得
	ScratchObj_GetObjPos( wk->p_d_card[pos], &now_x, &now_y );
	OS_Printf( "now_x = %d\n", now_x );
	OS_Printf( "now_y = %d\n", now_y );

	set_x = ( now_x + card_move_x_tbl[pos][wk->move_num] );
	set_y = ( now_y + card_move_y_tbl[wk->move_num] );
	wk->move_num++;

	OS_Printf( "set_x = %d\n", set_x );
	OS_Printf( "set_y = %d\n", set_y );
	ScratchObj_SetObjPos( wk->p_d_card[pos], set_x, set_y );

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	パレット切り替え
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void ScrPalChg( SCRATCH_WORK* wk, u32 frm, u8 pltt_no, u8 sx, u8 sy )
{
	OS_Printf( "切り替えるパレットナンバー = %d\n", pltt_no );
	GF_BGL_ScrPalChange( wk->bgl, frm, 0, 0, sx, sy, pltt_no );
	GF_BGL_LoadScreenV_Req( wk->bgl, frm );							//スクリーンデータ転送
	return;
}

//--------------------------------------------------------------
/**
 * @brief	銀箔に触れた数を取得
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"数"
 */
//--------------------------------------------------------------
static u8 Scratch_BlockFlagOnNumGet( SCRATCH_WORK* wk )
{
	u8 i,count;

	count = 0;

	for( i=0; i < SCRATCH_D_POKE_MAX ;i++ ){
		if( wk->block_flag[i] == 1 ){
			count++;
		}
	}

	return count;
}

//--------------------------------------------------------------
/**
 * @brief	銀箔に触れた数をクリア
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	void
 */
//--------------------------------------------------------------
static void Scratch_BlockFlagClear( SCRATCH_WORK* wk )
{
	u8 i;

	for( i=0; i < SCRATCH_D_POKE_MAX ;i++ ){
		wk->block_flag[i] = 0;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	触れた位置カウントをクリア
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	void
 */
//--------------------------------------------------------------
static void Scratch_BlockCountClear( SCRATCH_WORK* wk )
{
	u8 i;

	for( i=0; i < SCRATCH_D_POKE_MAX ;i++ ){
		wk->block_count[i] = 0;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	サンプリング情報を取得して格納
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	void
 */
//--------------------------------------------------------------
static void Scratch_HoseiTouch2( SCRATCH_WORK* wk, int index )
{
	int i,j,x,y;

	for( i=0; i < wk->tpData.Size ;i++ ){
		x = wk->tpData.TPDataTbl[i].x;
		y = wk->tpData.TPDataTbl[i].y;

		OS_Printf( "tpData.TPDataTbl[%d].x = %d\n", i, x );
		OS_Printf( "tpData.TPDataTbl[%d].y = %d\n", i, y );

		//block_hit_tblの中を参照出来ないので、同じデータテーブルを用意
		if( (block_hit_tbl2[index].left <= x) && (x <= block_hit_tbl2[index].right) &&
			(block_hit_tbl2[index].top <= y) && (y <= block_hit_tbl2[index].bottom) ){
			Scratch_Touch_2( wk, x, y );
		}
	}

	return;
}

//触れたフラグを立てる
static void Scratch_TouchFlagSet( SCRATCH_WORK* wk )
{
	int i;
	u8 x,y;

	for( i=0; i < wk->tpData.Size ;i++ ){

		x = wk->tpData.TPDataTbl[i].x;
		y = wk->tpData.TPDataTbl[i].y;

		//wk->touch_flag[ (x / 8) + ((y / 8)*32) ] |= (1 << (x % 8));
		
#if 0
		if( (x < SCRATCH_TOUCH_SIZE_X) && (y < SCRATCH_TOUCH_SIZE_Y) ){
			wk->touch_flag[ x + (y*SCRATCH_TOUCH_SIZE_X) ] = 1;
		}
#else
		Scratch_TouchFlagSetSub( wk, x, y );
#endif
	}

	return;
}

//触れた座標から4dotの幅を処理する
static void Scratch_TouchFlagSetSub( SCRATCH_WORK* wk, int tp_x, int tp_y )
{
	int i,j;

	for( j=-(DOT_WRITE_WIDTH); j < DOT_WRITE_WIDTH ;j++ ){			//Y
		for( i=-(DOT_WRITE_WIDTH); i < DOT_WRITE_WIDTH ;i++ ){		//X
			if( ((tp_x + i ) > 0) && ((tp_x + i) < SCRATCH_TOUCH_SIZE_X) &&
				((tp_y + j) > 0) && ((tp_y + j) < SCRATCH_TOUCH_SIZE_Y) ){
				//OS_Printf( "tp_x + i = %d\n", tp_x + i );
				//OS_Printf( "tp_y + j = %d\n", tp_y + j );
				wk->touch_flag[ (tp_x+i) + ((tp_y+j)*SCRATCH_TOUCH_SIZE_X) ] = 1;
			}
		}
	}
	return;
}

//触れたフラグチェック
static BOOL Scratch_TouchFlagCheck( SCRATCH_WORK* wk, u8 index )
{
	int i,j,start_x,start_y,count;

	count	= 0;
	start_x = block_check_pos[index].x;
	start_y = block_check_pos[index].y;

	//ブロック分のフラグをチェック
	for( i=start_y; i <  (start_y+BLOCK_SIZE_Y) ;i++ ){
		for( j=start_x; j <  (start_x+BLOCK_SIZE_X) ;j++ ){

			if( wk->touch_flag[i*SCRATCH_TOUCH_SIZE_X+j] == 1 ){
				count++;
			}
		}
	}

	OS_Printf( "count = %d\n", count );
	if( count >= BLOCK_COUNT ){

		//初回のみSEを鳴らす
		if( wk->block_count[index] == 0 ){
			Snd_SePlay( SE_D_KEZURI_OK );
			wk->kezuri_card[wk->kezuri_card_num] = index;	//どのカードか保存
			wk->kezuri_card_num++;

			//リーチにする
			//if( wk->card_poke[index] == wk->atari_poke ){
			//	wk->waku_flag = 1;
			//}
		}

		wk->block_count[index] = 1;
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	ウェイト計算してパレットナンバー、リソース変更
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	void
 */
//--------------------------------------------------------------
static void Scratch_PlttNoResChg( SCRATCH_WORK* wk )
{
	int i;

	wk->pltt_wait++;

	if( wk->pltt_wait >= WAKU_PAL_WAIT ){

		wk->pltt_wait = 0;

		wk->pltt_res_no++;
		if( wk->pltt_res_no >= WAKU_PAL_NUM ){
			wk->pltt_res_no = 0;
		}

		if( wk->kezuri_card_num == 2 ){

			//１枚目、２枚目
			for( i=0; i < 2; i++ ){
				ScratchObj_PaletteNoChg( wk->p_d_waku[i], 3 );
				ScratchObj_Vanish( wk->p_d_waku[i], SCRATCH_VANISH_OFF );	//表示
				ScratchObj_SetObjPos( wk->p_d_waku[i], 
						(d_poke_pos[ wk->kezuri_card[i] ].x + SCRATCH_GIN_WAKU_OFFSET_X), 
						d_poke_pos[ wk->kezuri_card[i] ].y + SCRATCH_GIN_WAKU_OFFSET_Y );
			}
		}

		//パレット3番使用変更
		//パレットリソースを切り替える
		ScratchClact_ButtonResourcePalChg( &wk->scratch_clact, wk->pltt_res_no );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	当たりがそろったか取得
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"TRUE = 当たり、FALSE = はずれ"
 */
//--------------------------------------------------------------
static BOOL Scratch_AtariCheck( SCRATCH_WORK* wk )
{
	int i,atari_flag;

	atari_flag = 0;

	//2枚は削らないと、リーチにはならない
	if( wk->kezuri_card_num <= 1 ){
		return FALSE;
	}

	if( wk->card_poke[ wk->kezuri_card[0] ] == wk->card_poke[ wk->kezuri_card[1] ] ){
		wk->waku_flag = 1;
	}else if( wk->card_poke[ wk->kezuri_card[0] ] == ANM_POKE_META ){	//メタモンは必ずリーチ
		wk->waku_flag = 1;
	}else if( wk->card_poke[ wk->kezuri_card[1] ] == ANM_POKE_META ){	//メタモンは必ずリーチ
		wk->waku_flag = 1;
	}
			
	//3枚は削らないと、当たりにはならない
	if( wk->kezuri_card_num <= 2 ){
		return FALSE;
	}

	//１、２、３枚目
	if( (wk->card_poke[ wk->kezuri_card[0] ] == wk->card_poke[ wk->kezuri_card[1] ]) &&
		(wk->card_poke[ wk->kezuri_card[0] ] == wk->card_poke[ wk->kezuri_card[2] ]) ){
		atari_flag = 1;
	}

	//１、２枚目(メタモン)
	if( (wk->card_poke[ wk->kezuri_card[0] ] == ANM_POKE_META) &&
		(wk->card_poke[ wk->kezuri_card[1] ] == ANM_POKE_META) ){
		atari_flag = 1;
	}

	//１、３枚目(メタモン)
	if( (wk->card_poke[ wk->kezuri_card[0] ] == ANM_POKE_META) &&
		(wk->card_poke[ wk->kezuri_card[2] ] == ANM_POKE_META) ){
		atari_flag = 1;
	}

	//２、３枚目(メタモン)
	if( (wk->card_poke[ wk->kezuri_card[1] ] == ANM_POKE_META) &&
		(wk->card_poke[ wk->kezuri_card[2] ] == ANM_POKE_META) ){
		atari_flag = 1;
	}

	//１(メタモン)、２、３枚目
	if( (wk->card_poke[ wk->kezuri_card[0] ] == ANM_POKE_META) &&
		(wk->card_poke[ wk->kezuri_card[1] ] == wk->card_poke[ wk->kezuri_card[2] ]) ){
		atari_flag = 1;
	}

	//１、２(メタモン)、３枚目
	if( (wk->card_poke[ wk->kezuri_card[1] ] == ANM_POKE_META) &&
		(wk->card_poke[ wk->kezuri_card[0] ] == wk->card_poke[ wk->kezuri_card[2] ]) ){
		atari_flag = 1;
	}

	//１、２、３(メタモン)枚目
	if( (wk->card_poke[ wk->kezuri_card[2] ] == ANM_POKE_META) &&
		(wk->card_poke[ wk->kezuri_card[0] ] == wk->card_poke[ wk->kezuri_card[1] ]) ){
		atari_flag = 1;
	}

	if( atari_flag == 1 ){
		//当たりポケモンをセットする
		for( i=0; i < SCRATCH_CARD_KEZURI_NUM; i++ ){
			if( wk->card_poke[ wk->kezuri_card[i] ] != ANM_POKE_META ){
				wk->atari_poke = wk->card_poke[ wk->kezuri_card[i] ];
				break;
			}
		}
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	当たりのメタモンのモザイクエフェクト：初期化
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 *
 * @return	"TRUE = メタモンいる、FALSE = メタモンいない"
 */
//--------------------------------------------------------------
static BOOL Scratch_MosaicEffInit( SCRATCH_WORK* wk )
{
	int i;
	BOOL flag;

	flag = FALSE;

	for( i=0; i < SCRATCH_CARD_ATARI_NUM ;i++ ){

		//メタモンだったらモザイクオン
		if( wk->card_poke[ wk->kezuri_card[i] ] == ANM_POKE_META ){
			ScratchObj_MosaicSet( wk->p_d_atari[i], TRUE );
			flag = TRUE;
		}
	}

	wk->mosaic_size = 0;					//モザイクサイズ(0-15)

	//モザイクの幅を設定
	G2_SetOBJMosaicSize( wk->mosaic_size, wk->mosaic_size );	//幅0-15(0は通常表示)

	return flag;
}

//--------------------------------------------------------------
/**
 * @brief	当たりのメタモンのモザイクエフェクト：メイン
 *
 * @param	wk		SCRATCH_WORK型のポインタ
 * @param	flag	0=mosaic++、1=mosaic--
 *
 * @return	"TRUE = 継続中、FALSE = 終了"
 */
//--------------------------------------------------------------
static BOOL Scratch_MosaicEffMain( SCRATCH_WORK* wk, u8 flag )
{
	int i;

	//メタモンがいるか
	for( i=0; i < SCRATCH_CARD_ATARI_NUM ;i++ ){
		if( wk->card_poke[ wk->kezuri_card[i] ] == ANM_POKE_META ){
			break;
		}
	}

	//メタモンいないので終了
	if( i == SCRATCH_CARD_ATARI_NUM ){
		return FALSE;
	}

	if( flag == 0 ){

		//初回のみ鳴らす
		if( wk->mosaic_size == 0 ){
			Snd_SePlay( SE_D_METAMON_CHG );
		}

		if( wk->mosaic_size < SCRATCH_MOSAIC_MAX ){
			wk->mosaic_size++;					//モザイクサイズ(0-15)
		}else{
			//メタモンを当たりポケモンに変更
			for( i=0; i < SCRATCH_CARD_ATARI_NUM ;i++ ){
				if( wk->card_poke[ wk->kezuri_card[i] ] == ANM_POKE_META ){
					ScratchObj_AnmChg(	wk->p_d_atari[i], wk->atari_poke );
				}
			}
			return FALSE;
		}
	}else{
		if( wk->mosaic_size > 0 ){
			wk->mosaic_size--;					//モザイクサイズ(0-15)
		}else{
			//モザイクをオフ
			for( i=0; i < SCRATCH_CARD_ATARI_NUM ;i++ ){
				ScratchObj_MosaicSet( wk->p_d_atari[i], FALSE );
			}
			return FALSE;
		}
	}

	//モザイクの幅を設定
	G2_SetOBJMosaicSize( wk->mosaic_size, wk->mosaic_size );	//幅0-15(0は通常表示)

	return TRUE;
}


//==============================================================================================
//
//	サブシーケンス
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	やめるへいくための準備
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_SeqSubYameruCard( SCRATCH_WORK* wk )
{
	wk->card_num++;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	つぎのカードへいくための準備
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_SeqSubNextCard( SCRATCH_WORK* wk )
{
	wk->card_num++;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	銀箔OK
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Scratch_SeqSubGinOk( SCRATCH_WORK* wk )
{
	ScratchObj_Vanish( wk->p_d_win, SCRATCH_VANISH_ON );				//オフ
	GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_START], FBMP_COL_NULL );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_START] );
	return;
}


//==============================================================================================
//
//	ヒットチェック(タッチパネル)
//
//==============================================================================================
#if 0
//--------------------------------------------------------------
/**
 * @brief	ヒットチェック
 *
 * @param	none
 *
 * @return	"TRUE=触れている、FALSE=触れていない"
 */
//--------------------------------------------------------------
static BOOL Scratch_TPSystemHitCheck( ANIMAL_OBJ* animal_obj )
{
	int i;
	RECT_HIT_TBL hit_tbl[2];
	const VecFx32* mat_p;
	VecFx32 mat;

	for( i=0; i < 2 ;i++ ){
		hit_tbl[i].rect.top		= TP_HIT_END;			//初期化
		hit_tbl[i].rect.bottom	= 0;
		hit_tbl[i].rect.left	= 0;
		hit_tbl[i].rect.right	= 0;
	}

	mat_p = AnimalObj_GetPos( animal_obj );				//動物の位置取得
	mat = *mat_p;

#if SUB_DISP_CHG										//1=サブ画面に変更
	//mat.y -= SUB_SURFACE_Y;
	mat.y -= SCRATCH_SUB_ACTOR_DISTANCE;
#endif

	//動物は「32x32」
	hit_tbl[0].rect.top		= mat.y / FX32_ONE - 16;
	hit_tbl[0].rect.bottom	= mat.y / FX32_ONE + 16;
	hit_tbl[0].rect.left	= mat.x / FX32_ONE - 16;
	hit_tbl[0].rect.right	= mat.x / FX32_ONE + 16;

	if( GF_TP_SingleHitCont((const RECT_HIT_TBL*)hit_tbl) == TRUE ){
		Snd_SePlay( SE_D_BUTTON_TOUCH );
		//OS_Printf( "テーブルにHITしました\n" );
		return TRUE;
	}

	return FALSE;
}
#endif


//==============================================================================================
//
//	キャラクターデータをコピーして、キャラクタデータを書き換える
//
//==============================================================================================

//------------------------------------------------------------------
/**
 * キャラクターデータをバッファにコピー
 * "終了時に開放が必要"
 *
 * @param   wk		SCRATCH_WORK型のポインタ
 *
 * キャラを被らないように画面サイズ分用意しておく、
 * それをこの関数でバッファにコピーしておいて、
 * 画面をタッチしたら、バッファのその場所を0クリアする(ビット単位)
 */
//------------------------------------------------------------------
static void Scratch_Touch_0( SCRATCH_WORK* wk )
{
	//キャラクタ取得
	wk->p_char = ArcUtil_CharDataGet(	ARC_SCRATCH, NARC_scratch_l_bg01gin_NCGR, 
	//wk->p_char = ArcUtil_CharDataGet(	ARC_SCRATCH, NARC_scratch_l_bg00_NCGR, 
										0, &wk->p_chardata, HEAPID_SCRATCH );

	//キャラクタ転送
	GF_BGL_LoadCharacter(	wk->bgl, SCRATCH_FRAME_D_TOUCH,
							wk->p_chardata->pRawData,
							wk->p_chardata->szByte,
							0 );

	GF_BGL_LoadScreenReq( wk->bgl, SCRATCH_FRAME_D_TOUCH );
	return;
}

//------------------------------------------------------------------
/**
 * バッファを生成	"終了時に開放が必要"
 *
 * @param   wk		SCRATCH_WORK型のポインタ
 */
//------------------------------------------------------------------
static void Scratch_Touch_1( SCRATCH_WORK* wk )
{
	wk->buf = sys_AllocMemory( HEAPID_SCRATCH, wk->p_chardata->szByte );
	if( wk->buf == NULL ){
		OS_Printf( "メモリ確保失敗！\n" );
		GF_ASSERT( wk->buf != NULL );
	}
	return;
}

//------------------------------------------------------------------
/**
 * キャラクターデータをコピー
 *
 * @param   wk		SCRATCH_WORK型のポインタ
 */
//------------------------------------------------------------------
static void Scratch_Touch_1_5( SCRATCH_WORK* wk )
{
	//OS_Printf( "キャラクタデータサイズ = %d\n", wk->p_chardata->szByte );
	memcpy( wk->buf, wk->p_chardata->pRawData, wk->p_chardata->szByte );
	return;
}

//------------------------------------------------------------------
/**
 * タッチパネルに触れている座標を取得してキャラクターデータを書き換える
 *
 * @param   wk		SCRATCH_WORK型のポインタ
 */
//------------------------------------------------------------------
static void Scratch_Touch_2( SCRATCH_WORK* wk, int tp_x, int tp_y )
{
	int i,j;

	//これで正確か確かめる
	//OS_Printf( "tp_x = %d\n", tp_x );
	//OS_Printf( "tp_y = %d\n", tp_y );
	//Scratch_Touch_3( wk, tp_x, tp_y );

	for( j=-(DOT_WRITE_WIDTH); j < DOT_WRITE_WIDTH ;j++ ){			//Y
		for( i=-(DOT_WRITE_WIDTH); i < DOT_WRITE_WIDTH ;i++ ){		//X
			if( ((tp_x + i ) > 0) && ((tp_x + i) < 256) &&
				((tp_y + j) > 0) && ((tp_y + j) < 192) ){
				//OS_Printf( "tp_x + i = %d\n", tp_x + i );
				//OS_Printf( "tp_y + j = %d\n", tp_y + j );
				Scratch_Touch_3( wk, (tp_x+i), (tp_y+j) );
			}
		}
	}

	GF_BGL_LoadCharacter( wk->bgl, SCRATCH_FRAME_D_TOUCH, wk->buf, wk->p_chardata->szByte, 0 );
	GF_BGL_LoadScreenReq( wk->bgl, SCRATCH_FRAME_D_TOUCH );
	return;
}

//------------------------------------------------------------------
/**
 * 渡された位置のキャラクターデータを0クリア
 *
 * @param   wk		SCRATCH_WORK型のポインタ
 * @param   tp_x	ドット単位のX
 * @param   tp_y	ドット単位のY
 */
//------------------------------------------------------------------
static void Scratch_Touch_3( SCRATCH_WORK* wk, u32 tp_x, u32 tp_y )
{
	u8 set_bit;
	u32 pos_x,pos_y,offset_x,offset_y,decide_pos;

	set_bit = 0xff;
		
	if( (tp_x % 2) == 0 ){					//シフトする値(前半4bitか、後半4bitか)
		set_bit ^= 0x0f;					//2進(00001111)
	}else{
		set_bit ^= 0xf0;					//2進(11110000)
	}
	//OS_Printf( "set_bit = %d\n", set_bit );

	//キャラ単位ごとの場所を取得
	pos_x = ( tp_x / 8 * ONE_CHAR );		//ドット位置をキャラ位置にして、1キャラ
	pos_y = ( tp_y / 8 * ONE_CHAR * 32 );	//ドット位置をキャラ位置にして、1キャラx横分
	//OS_Printf( "pos_x = %d\n", pos_x );
	//OS_Printf( "pos_y = %d\n", pos_y );

	offset_x = ( tp_x % 8 / 2 );			//横0-3のどれか
	//OS_Printf( "offset_x = %d\n", offset_x );
	
	if( tp_y < 8 ){
		offset_y = ( tp_y * 4 );			//縦0-7のどれか(4=横の要素数)
	}else{
		offset_y = ( tp_y % 8 );
		offset_y = ( offset_y * 4 );		//縦0-7のどれか(4=横の要素数)
	}
	//OS_Printf( "offset_y = %d\n", offset_y );

	decide_pos = ( pos_y + pos_x + offset_x + offset_y );
	//OS_Printf( "セットしている要素数：decide_pos = %d\n", decide_pos );

	//OS_Printf( "前 wk->buf = %d\n", wk->buf[decide_pos] );
	wk->buf[ decide_pos ] &= set_bit;
	//OS_Printf( "後 wk->buf = %d\n\n******************\n", wk->buf[decide_pos] );
	return;
}


//==============================================================================================
//
//	左上から右に16キャラ分(COL_NUM分)パレットのカラーと同じ8x8のキャラに書き込み
//
//==============================================================================================
#if 0
#define COL_NUM	(2)

void CgxDataSetup( GF_BGL_INI* bgl )
{
	int i;
	u32 fillDat,offset;
	u8* buf = sys_AllocMemory( HEAPID_SCRATCH, (0x20 * COL_NUM) );

	offset = 0;

	if( buf ){

		for( i=0; i < COL_NUM; i++ ){
			fillDat = (i<<28) | (i<<24) | (i<<20) | (i<<16) | (i<<12) | (i<<8) | (i<<4) | i;
			MI_CpuFillFast( buf+i*0x20, fillDat, 0x20 );
		}

		GF_BGL_LoadCharacter( bgl, SCRATCH_FRAME_D_TOUCH, buf, (0x20 * COL_NUM), offset );
		sys_FreeMemoryEz( buf );
	}

	return;
}
#endif


