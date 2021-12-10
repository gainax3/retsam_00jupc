//==============================================================================
/**
 * @file	footprint_main.c
 * @brief	足跡ボードメイン
 * @author	matsuda
 * @date	2008.01.18(金)
 */
//==============================================================================
#include "common.h"
#include "system/snd_tool.h"
#include "system/bmp_list.h"
#include "system/pmfprint.h"
#include "savedata/system_data.h"
#include "system/bmp_menu.h"
#include "system/procsys.h"
#include "system/wipe.h"
#include "system/window.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "msgdata/msg.naix"
#include "system/fontproc.h"
#include "gflib/strbuf_family.h"
#include "msgdata/msg_wflby_footprint.h"
#include "system/font_arc.h"
#include "savedata/config.h"
#include "communication/wm_icon.h"
#include "system/fontoam.h"
#include "system/d3dobj.h"
#include "system/clact_tool.h"

#include "application/wifi_lobby/wflby_system_def.h"
#include "application/footprint_main.h"
#include "footprint_common.h"
#include "graphic/footprint_board.naix"
#include "footprint_stamp.h"
#include "footprint_comm.h"
#include "footprint_tool.h"
#include "footprint_id.h"
#include "footprint_control.h"
#include "footprint_snd_def.h"

#include "battle/battle_common.h"	//POKEMON_TEMOTI_MAX定義
#include "poketool/poke_tool.h"
#include "poketool/pokeparty.h"
#include "poketool/pokefoot.h"	//POKEFOOT_ARC_CHAR_DMMY定義の為
#include "system/touch_subwindow.h"


//==============================================================================
//	定数定義
//==============================================================================
///HEAPID_FOOTPRINTが確保するヒープサイズ
#define FOOTPRINT_HEAP_SIZE		(0x50000)

///管理するフォントOAM数
#define FOOT_FONTOAM_MAX		(4)	//少し多めに

//--------------------------------------------------------------
//	カメラ設定
//--------------------------------------------------------------
#define FOOTPRINT_CAMERA_MODE			GF_CAMERA_PERSPECTIV	//(GF_CAMERA_ORTHO)

#define FOOTPRINT_CAMERA_PERSPWAY		(FX_GET_ROTA_NUM(22))
#define FOOTPRINT_CAMERA_NEAR			( FX32_ONE )
#define FOOTPRINT_CAMERA_FAR			( FX32_ONE * 900 )
#define FOOTPRINT_CAMERA_TX				( 0 )		/// target
#define FOOTPRINT_CAMERA_TY				( -FX32_ONE * 8 )
#define FOOTPRINT_CAMERA_TZ				( 0 )

///カメラの注視点までの距離
#define FOOTPRINT_CAMERA_DISTANCE		(0x7c000)	//(0x96 << FX32_SHIFT)

///カメラアングル
static const CAMERA_ANGLE FootprintCameraAngle = {
	FX_GET_ROTA_NUM(0), FX_GET_ROTA_NUM(0), FX_GET_ROTA_NUM(0),
};

//--------------------------------------------------------------
//	ボードのモデル設定
//--------------------------------------------------------------
#define BOARD_X		(FX32_CONST(0))
#define BOARD_Y		(FX32_CONST(0))
#define BOARD_Z		(FX32_CONST(0))
#define BOARD_SCALE	(FX32_CONST(1.00f))

//--------------------------------------------------------------
//	CL_ACT用の定数定義
//--------------------------------------------------------------
///メイン	OAM管理領域・開始
#define FOOTPRINT_OAM_START_MAIN			(0)
///メイン	OAM管理領域・終了
#define FOOTPRINT_OAM_END_MAIN				(128)
///メイン	アフィン管理領域・開始
#define FOOTPRINT_OAM_AFFINE_START_MAIN		(0)
///メイン	アフィン管理領域・終了
#define FOOTPRINT_OAM_AFFINE_END_MAIN		(32)
///サブ	OAM管理領域・開始
#define FOOTPRINT_OAM_START_SUB				(0)
///サブ	OAM管理領域・終了
#define FOOTPRINT_OAM_END_SUB				(128)
///サブ アフィン管理領域・開始
#define FOOTPRINT_OAM_AFFINE_START_SUB		(0)
///サブ	アフィン管理領域・終了
#define FOOTPRINT_OAM_AFFINE_END_SUB		(32)

///キャラマネージャ：キャラクタID管理数(上画面＋下画面)
#define FOOTPRINT_CHAR_MAX					(48 + 48)
///キャラマネージャ：メイン画面サイズ(byte単位)
#define FOOTPRINT_CHAR_VRAMSIZE_MAIN		(1024 * 0x40)	//64K
///キャラマネージャ：サブ画面サイズ(byte単位)
#define FOOTPRINT_CHAR_VRAMSIZE_SUB			(512 * 0x20)	//32K

///メイン画面＋サブ画面で使用するアクター総数
#define FOOTPRINT_ACTOR_MAX					(64 + 64)	//メイン画面 + サブ画面

///OBJで使用するパレット本数(上画面＋下画面)
#define FOOTPRINT_OAM_PLTT_MAX				(16 + 16)

///OAMリソース：キャラ登録最大数(メイン画面 + サブ画面)
#define FOOTPRINT_OAMRESOURCE_CHAR_MAX		(FOOTPRINT_CHAR_MAX)
///OAMリソース：パレット登録最大数(メイン画面 + サブ画面)
#define FOOTPRINT_OAMRESOURCE_PLTT_MAX		(FOOTPRINT_OAM_PLTT_MAX)
///OAMリソース：セル登録最大数
#define FOOTPRINT_OAMRESOURCE_CELL_MAX		(64)
///OAMリソース：セルアニメ登録最大数
#define FOOTPRINT_OAMRESOURCE_CELLANM_MAX	(64)
///OAMリソース：マルチセル登録最大数
#define FOOTPRINT_OAMRESOURCE_MCELL_MAX		(8)
///OAMリソース：マルチセルアニメ登録最大数
#define FOOTPRINT_OAMRESOURCE_MCELLANM_MAX	(8)

//--------------------------------------------------------------
//	システム
//--------------------------------------------------------------
///参加者リストを更新する時間間隔
#define ENTRY_LIST_UPDATE_TIME				(30)

///タイムアップしてから終了までの待ち時間
#define FOOT_TIMEUP_WAIT					(90)

//--------------------------------------------------------------
//	アクター
//--------------------------------------------------------------
///インクのX座標開始位置
#define INK_POS_START_X		(16)
///インクの隣り合うインクとの配置間隔X
#define INK_POS_SPACE_X		(32)
///インクのY座標
#define INK_POS_Y			(176)

///インクの下地のX座標開始位置
#define INK_FOUNDATION_POS_START_X		(INK_POS_START_X)
///インクの下地の隣り合うインクの下地との配置間隔X
#define INK_FOUNDATION_POS_SPACE_X		(INK_POS_SPACE_X)
///インクの下地のY座標
#define INK_FOUNDATION_POS_Y			(INK_POS_Y)

///インクの足跡のX座標開始位置
#define INK_FOOT_POS_START_X		(INK_POS_START_X)
///インクの足跡の隣り合うインクの足跡との配置間隔X
#define INK_FOOT_POS_SPACE_X		(INK_POS_SPACE_X)
///インクの足跡のY座標
#define INK_FOOT_POS_Y			(INK_POS_Y)

///タッチエフェクトを同時に出せる最大数
#define TOUCH_EFF_MAX			(3)

//--------------------------------------------------------------
//	スクリーン
//--------------------------------------------------------------
///インクの枠のX座標開始位置
#define SCRN_INK_POS_START_X		(0)
///インクの枠のY座標
#define SCRN_INK_POS_Y			(0x14)
///インクの枠の隣り合うインクの枠との配置間隔X
#define SCRN_INK_POS_SIZE_X		(4)
///インクの枠の隣り合うインクの枠との配置間隔X
#define SCRN_INK_POS_SIZE_Y		(4)

///サブ画面：名前リストのパレット色開始カラー位置
#define SUBBG_LIST_COLOR_START	(2 * 16 + 1)

///名前表示リストのスクリーンサイズ
#define NAMELIST_SCRN_SIZE		(0x800)

//--------------------------------------------------------------
//	「やめる」ボタン
//--------------------------------------------------------------
///「やめる」ボタンのBGカラー位置
#define EXIT_BUTTON_COLOR_POS		(0*16 + 9)
///「やめる」ボタンを押した時にかけるフェードevy値
#define EXIT_BUTTON_COLOR_EVY		(8)
///「やめる」ボタンを押した時にかけるフェードのカラーコード
#define EXIT_BUTTON_COLOR_CODE		(0x0000)

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
///FootPrintTool_NameAllUpdate関数の戻り値
typedef enum{
	FOOTPRINT_NAME_UPDATE_STATUS_NULL,			///<変化なし
	FOOTPRINT_NAME_UPDATE_STATUS_ENTRY,			///<新規エントリー発生
	FOOTPRINT_NAME_UPDATE_STATUS_LEAVE_ROOM,	///<退室者発生
}FOOTPRINT_NAME_UPDATE_STATUS;


//--------------------------------------------------------------
//	
//--------------------------------------------------------------
enum{
	FONTOAM_LEFT,		///<X左端座標
	FONTOAM_CENTER,		///<X中心座標
};


//==============================================================================
//	構造体定義
//==============================================================================
///ボード制御構造体
typedef struct{
	D3DOBJ_MDL  mdl;
	D3DOBJ      obj;
}BOARD_PARAM;

///フォントアクターワーク
typedef struct{
	FONTOAM_OBJ_PTR fontoam;
	CHAR_MANAGER_ALLOCDATA cma;
	u16 font_len;
}FONT_ACTOR;


///足跡ボード制御構造体
typedef struct _FOOTPRINT_SYS{
	FOOTPRINT_PARAM *parent_work;		///<parent_work
	SAVEDATA *sv;						///<セーブデータへのポインタ
	
	GF_BGL_INI		*bgl;				///<BGシステムへのポインタ
	PALETTE_FADE_PTR pfd;				///<パレットシステム
	FONTOAM_SYS_PTR fontoam_sys;		///<フォントOAMシステムへのポインタ
	GF_G3DMAN *g3Dman;
	TCB_PTR update_tcb;					///<Update用TCBへのポインタ
	CATS_SYS_PTR		csp;
	CATS_RES_PTR		crp;
	
	TOUCH_SW_SYS *yesno_button;			///<ボタンの「はい・いいえ」システム
	u8 yesno_button_use;				///<TRUE:「はい・いいえ」ボタンを使用中
	u8 timeup_wait;						///<タイムアップしてから終了までの待ち時間
	
	// 描画まわりのワーク（主にBMP用の文字列周り）
	WORDSET			*wordset;							// メッセージ展開用ワークマネージャー
	MSGDATA_MANAGER *msgman;						// 名前入力メッセージデータマネージャー

	// BMPWIN描画周り
	GF_BGL_BMPWIN		name_win[FOOTPRINT_BMPWIN_NAME_MAX]; //名前表示用のBMPWIN
	GF_BGL_BMPWIN		talk_win;					 //会話メッセージ用のBMPWIN
	STRBUF *talk_strbuf;				///<会話メッセージ用バッファ
	u8 msg_index;						///<メッセージインデックス

	GF_CAMERA_PTR camera;				///<カメラへのポインタ
	fx32 world_width;					///<設置されているカメラに表示されているワールド座標の幅
	fx32 world_height;					///<設置されているカメラに表示されているワールド座標の高さ
	
	//ボード
	BOARD_PARAM board;					///<ボード制御ワーク
	
	ARCHANDLE *handle_footprint;		///<footprint_board.narcのハンドル
	ARCHANDLE *handle_footmark;			///<足跡グラフィックのハンドル
	
	FOOTPRINT_MY_COMM_STATUS my_comm_status;	///<自分の通信ステータス
	
	s32 entry_userid[FOOTPRINT_ENTRY_MAX];		///<参加者のユーザーIDリスト
	int entry_list_update_timer;			///<参加者のユーザーIDリストを更新するタイムをカウント
	
	STAMP_SYSTEM_WORK ssw;				///<スタンプシステムワーク
	STAMP_PARAM my_stamp_param[POKEMON_TEMOTI_MAX];		///<自分の手持ちスタンプパラメータ
	u8 select_no;						///<選択している足跡スタンプの番号
	u8 yameru_pal_pos;					///<「やめる」フォントOAMのパレット番号
	
	CATS_ACT_PTR cap_ink[POKEMON_TEMOTI_MAX];	///<インクアクター
	CATS_ACT_PTR cap_ink_foundation[POKEMON_TEMOTI_MAX];	///<インクの下地アクター
	CATS_ACT_PTR cap_ink_foot[POKEMON_TEMOTI_MAX];	///<インクの上に配置する足跡アクター
	CATS_ACT_PTR cap_name_frame;				///<名前を囲む枠アクター
	CATS_ACT_PTR cap_name_foot[FOOTPRINT_ENTRY_MAX];	///<名前の横の足跡アクター
	CATS_ACT_PTR cap_touch_eff[TOUCH_EFF_MAX];	///<タッチエフェクトアクター
	
	u16 name_foot_monsno[FOOTPRINT_ENTRY_MAX];	///<名前の横に出しているポケモン番号
	u16 name_foot_color[FOOTPRINT_ENTRY_MAX];	///<名前の横に出しているカラー
	
	int game_status;					///<ゲーム状態(FOOTPRINT_GAME_STATUS_???)
	
	u16 namelist_scrn[NAMELIST_SCRN_SIZE];			///<サブ画面の名前表示リストのスクリーンデータ
	
	FONT_ACTOR fontoam_exit;			///<「やめる」

	int ink_now;						///<インクの現在の残り
	int ink_calc;						///<インクゲージの現在計算位置
	int ink_sub;						///<インクの減算量
	
	BOOL arceus_flg;					///<TRUE:アルセウスOK
}FOOTPRINT_SYS;


//==============================================================================
//	CLACT用データ
//==============================================================================
static	const TCATS_OAM_INIT FootprintTcats = {
	FOOTPRINT_OAM_START_MAIN, FOOTPRINT_OAM_END_MAIN,
	FOOTPRINT_OAM_AFFINE_START_MAIN, FOOTPRINT_OAM_AFFINE_END_MAIN,
	FOOTPRINT_OAM_START_SUB, FOOTPRINT_OAM_END_SUB,
	FOOTPRINT_OAM_AFFINE_START_SUB, FOOTPRINT_OAM_AFFINE_END_SUB,
};

static	const TCATS_CHAR_MANAGER_MAKE FootprintCcmm = {
	FOOTPRINT_CHAR_MAX,
	FOOTPRINT_CHAR_VRAMSIZE_MAIN,
	FOOTPRINT_CHAR_VRAMSIZE_SUB,
	GX_OBJVRAMMODE_CHAR_1D_128K,	//64K	カウントダウンエフェクトが128KOBJなので合わせた
	GX_OBJVRAMMODE_CHAR_1D_32K
};

static const TCATS_RESOURCE_NUM_LIST FootprintResourceList = {
	FOOTPRINT_OAMRESOURCE_CHAR_MAX,
	FOOTPRINT_OAMRESOURCE_PLTT_MAX,
	FOOTPRINT_OAMRESOURCE_CELL_MAX,
	FOOTPRINT_OAMRESOURCE_CELLANM_MAX,
	FOOTPRINT_OAMRESOURCE_MCELL_MAX,
	FOOTPRINT_OAMRESOURCE_MCELLANM_MAX,
};


//==============================================================================
//	データ
//==============================================================================
///参加者の名前表示位置(BMPWINの位置)
ALIGN4 static const u8 NameBmpwinPos[][2] = {	//X,Y
	{7,		5},
	{0x16,	5},
	{7,		5 + 5*1},
	{0x16,	5 + 5*1},
	{7,		5 + 5*2},
	{0x16,	5 + 5*2},
	{7,		5 + 5*3},
	{0x16,	5 + 5*3},
};

///サブ画面：名前横の足跡アクターの座標
ALIGN4 static const s16 Sub_FootmarkPos[][2] = {	//x, y
	{32,		48},
	{0x13*8,	48},
	{32,		48 + 5*8},
	{0x13*8,	48 + 5*8},
	{32,		48 + 10*8},
	{0x13*8,	48 + 10*8},
	{32,		48 + 15*8},
	{0x13*8,	48 + 15*8},
};

///サブ画面：自分を表す名前を囲むフレームのアクター座標
ALIGN4 static const s16 Sub_NameFramePos[][2] = {	//x, y
	{9*8,		6*8 + 5*8*0},
	{0x18*8,	6*8 + 5*8*0},
	{9*8,		6*8 + 5*8*1},
	{0x18*8,	6*8 + 5*8*1},
	{9*8,		6*8 + 5*8*2},
	{0x18*8,	6*8 + 5*8*2},
	{9*8,		6*8 + 5*8*3},
	{0x18*8,	6*8 + 5*8*3},
};

///サブ画面：名前リストの矩形範囲(スクリーン)
ALIGN4 static const u16 Sub_ListScrnRange[][4] = {	//x, y, size_x, size_y
	{2,			4 + 5*0,		14, 4},
	{0x11,		4 + 5*0,		14, 4},
	{2,			4 + 5*1,		14, 4},
	{0x11,		4 + 5*1,		14, 4},
	{2,			4 + 5*2,		14, 4},
	{0x11,		4 + 5*2,		14, 4},
	{2,			4 + 5*3,		14, 4},
	{0x11,		4 + 5*3,		14, 4},
};

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
///手持ちポケモンがいない時にインクパレットを埋めるスクリーンコード
static const u16 MyInkPaletteEraseScrnCode[] = {	//1段目、2段目…
	0x15, 0x35, 0x35, 0x55,
};

//==============================================================================
//	アクターヘッダ
//==============================================================================
///インク アクターヘッダ
static const TCATS_OBJECT_ADD_PARAM_S InkObjParam = {
	0,0, 0,		//x, y, z
	0, SOFTPRI_INK, PALOFS_INK,		//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_INK,			//キャラ
		PLTTID_OBJ_COMMON,	//パレット
		CELLID_INK,			//セル
		CELLANMID_INK,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	ACTBGPRI_INK,			//BGプライオリティ
	0,			//Vram転送フラグ
};

///インクの下地 アクターヘッダ
static const TCATS_OBJECT_ADD_PARAM_S InkFoundationObjParam = {
	0,0, 0,		//x, y, z
	0, SOFTPRI_INK_FOUNDATION, PALOFS_INK_FOUNDATION,		//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_INK_FOUNDATION,			//キャラ
		PLTTID_OBJ_COMMON,	//パレット
		CELLID_INK_FOUNDATION,			//セル
		CELLANMID_INK_FOUNDATION,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	ACTBGPRI_INK_FOUNDATION,			//BGプライオリティ
	0,			//Vram転送フラグ
};

///インクの上に配置する足跡 アクターヘッダ
static const TCATS_OBJECT_ADD_PARAM_S InkFootObjParam = {
	0,0, 0,		//x, y, z
	0, SOFTPRI_INK_FOOT, 0,		//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_INK_FOOT_0,			//キャラ
		PLTTID_OBJ_INK_FOOT,	//パレット
		CELLID_INK_FOOT,			//セル
		CELLANMID_INK_FOOT,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	ACTBGPRI_INK_FOOT,			//BGプライオリティ
	0,			//Vram転送フラグ
};

///インクパレットをタッチした時に出すエフェクト アクターヘッダ
static const TCATS_OBJECT_ADD_PARAM_S TouchEffObjParam = {
	0,0, 0,		//x, y, z
	0, SOFTPRI_TOUCH_EFF, PALOFS_TOUCH_EFF,		//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_TOUCH_EFF,			//キャラ
		PLTTID_OBJ_COMMON,	//パレット
		CELLID_TOUCH_EFF,			//セル
		CELLANMID_TOUCH_EFF,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	ACTBGPRI_TOUCH_EFF,			//BGプライオリティ
	0,			//Vram転送フラグ
};

///名前を囲む枠 アクターヘッダ
static const TCATS_OBJECT_ADD_PARAM_S NameFrameObjParam = {
	0,0, 0,		//x, y, z
	0, SOFTPRI_SUB_NAME_FRAME, PALOFS_SUB_NAME_FRAME,	//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DSUB,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_SUB_NAME_FRAME,			//キャラ
		PLTTID_SUB_OBJ_COMMON,			//パレット
		CELLID_SUB_NAME_FRAME,			//セル
		CELLANMID_SUB_NAME_FRAME,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	ACTBGPRI_SUB_NAME_FRAME,			//BGプライオリティ
	0,			//Vram転送フラグ
};

///名前の横に出す足跡 アクターヘッダ
static const TCATS_OBJECT_ADD_PARAM_S NameFootObjParam = {
	0,0, 0,		//x, y, z
	0, SOFTPRI_SUB_NAME_FOOT, PALOFS_SUB_NAME_FOOT,	//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DSUB,		//描画エリア
	{	//使用リソースIDテーブル
		CHARID_SUB_NAME_FOOT_0,			//キャラ
		PLTTID_SUB_OBJ_COMMON,			//パレット
		CELLID_SUB_NAME_FOOT,			//セル
		CELLANMID_SUB_NAME_FOOT,		//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	ACTBGPRI_SUB_NAME_FOOT,			//BGプライオリティ
	0,			//Vram転送フラグ
};

//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void Footprint_Update(TCB_PTR tcb, void *work);
static void VBlankFunc( void * work );
static void FootPrint_VramBankSet(GF_BGL_INI *bgl);
static void BgExit( GF_BGL_INI * ini );
static void BgGraphicSet( FOOTPRINT_SYS * fps, ARCHANDLE* p_handle );
static void BmpWinInit( FOOTPRINT_SYS *fps );
static void BmpWinDelete( FOOTPRINT_SYS *fps );
static GF_G3DMAN * Footprint_3D_Init(int heap_id);
static void FootprintSimpleSetUp(void);
static void Footprint_3D_Exit(GF_G3DMAN *g3Dman);
static void Footprint_CameraInit(FOOTPRINT_SYS *fps);
static void Footprint_CameraExit(FOOTPRINT_SYS *fps);
static void Model3DSet( FOOTPRINT_SYS * fps, ARCHANDLE* p_handle );
static void Model3DDel(FOOTPRINT_SYS *fps);
static void Model3D_Update(FOOTPRINT_SYS *fps);
static void Debug_CameraMove(FOOTPRINT_SYS *fps);
static void Footprint_MyCommStatusSet(FOOTPRINT_SYS *fps);
static void Footprint_InParamCreate(FOOTPRINT_SYS_PTR fps, FOOTPRINT_IN_PARAM *in_para);
static void Footprint_Temoti_to_StampParam(int board_type,SAVEDATA * sv, STAMP_PARAM *stamp_array);
static void DefaultResourceSet_Main(FOOTPRINT_SYS *fps, ARCHANDLE *hdl_main);
static void DefaultResourceSet_Sub(FOOTPRINT_SYS *fps, ARCHANDLE *hdl_main);
static void DefaultActorSet_Main(FOOTPRINT_SYS *fps);
static void DefaultActorDel_Main(FOOTPRINT_SYS *fps);
static void DefaultActorSet_Sub(FOOTPRINT_SYS *fps);
static void DefaultActorDel_Sub(FOOTPRINT_SYS *fps);
static void MyInkPaletteSettings(FOOTPRINT_SYS *fps);
static BOOL OBJFootCharRewrite(int monsno, int form_no, CATS_ACT_PTR cap, ARCHANDLE *hdl_main, ARCHANDLE *hdl_mark, NNS_G2D_VRAM_TYPE vram_type, BOOL arceus_flg);
static void Footprint_TouchEffAdd(FOOTPRINT_SYS_PTR fps, int hit_pos);
static void Footprint_SelectInkPaletteFade(FOOTPRINT_SYS_PTR fps, int hit_pos);
static void Footprint_TouchEffUpdate(FOOTPRINT_SYS_PTR fps);
static FOOTPRINT_NAME_UPDATE_STATUS FootPrintTool_NameAllUpdate(FOOTPRINT_SYS *fps);
static void Sub_FontOamCreate(FOOTPRINT_SYS_PTR fps, FONT_ACTOR *font_actor, const STRBUF *str, 
	FONT_TYPE font_type, GF_PRINTCOLOR color, int pal_offset, int pal_id, 
	int x, int y, int pos_center);
static void Sub_FontOamDelete(FONT_ACTOR *font_actor);
static void FontLenGet(const STRBUF *str, 
	FONT_TYPE font_type, int *ret_dot_len, int *ret_char_len);
static void Footprint_InkGaugeUpdate(FOOTPRINT_SYS_PTR fps);
static BOOL Footprint_InkGauge_Consume(FOOTPRINT_SYS_PTR fps, int consume_num);


//==============================================================================
//	デバッグ用変数など
//==============================================================================
#ifdef PM_DEBUG
//動作タイプ順に並べた性格値
static const u32 DebugSeikakuTbl[] = {1, 14, 6, 5, 0, 3, 8, 9, 18, 15};

///デバッグ用ワーク
static struct{
	u8 occ_seikaku;		//性格制御有効
	u8 move_type;		//スタンプの動作タイプ
	u8 consume_zero;	//TRUE=インクゲージが減らなくなる
	u32 backup_personal_rnd;
}DebugFoot = {0};
#endif



//--------------------------------------------------------------
/**
 * @brief   プロセス関数：初期化
 *
 * @param   proc		プロセスデータ
 * @param   seq			シーケンス
 *
 * @retval  処理状況
 */
//--------------------------------------------------------------
PROC_RESULT FootPrintProc_Init( PROC * proc, int * seq )
{
	FOOTPRINT_SYS *fps;
	
	sys_VBlankFuncChange(NULL, NULL);	// VBlankセット
	sys_HBlankIntrStop();	//HBlank割り込み停止

	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
	GX_SetVisiblePlane(0);
	GXS_SetVisiblePlane(0);
	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
	G2_BlendNone();
	G2S_BlendNone();

	sys_CreateHeap(HEAPID_BASE_APP, HEAPID_FOOTPRINT, FOOTPRINT_HEAP_SIZE);

	fps = PROC_AllocWork(proc, sizeof(FOOTPRINT_SYS), HEAPID_FOOTPRINT );
	MI_CpuClear8(fps, sizeof(FOOTPRINT_SYS));
	fps->parent_work = PROC_GetParentWork(proc);
#ifdef PM_DEBUG
	if(fps->parent_work->wflby_sys == NULL){
		fps->sv = fps->parent_work->debug_sv;
	}
	else{
		fps->sv = WFLBY_SYSTEM_GetSaveData(fps->parent_work->wflby_sys);
	}
#else
	fps->sv = WFLBY_SYSTEM_GetSaveData(fps->parent_work->wflby_sys);
#endif
	
	fps->arceus_flg = WFLBY_SYSTEM_FLAG_GetArceus(fps->parent_work->wflby_sys);
	Footprint_MyCommStatusSet(fps);
	Footprint_Temoti_to_StampParam(fps->parent_work->board_type, fps->sv, fps->my_stamp_param);
	fps->ink_now = INK_GAUGE_TANK_MAX;
	fps->ink_calc = fps->ink_now;
	
//	simple_3DBGInit(HEAPID_FOOTPRINT);
	fps->g3Dman = Footprint_3D_Init(HEAPID_FOOTPRINT);

	//パレットフェードシステム作成
	fps->pfd = PaletteFadeInit(HEAPID_FOOTPRINT);
	PaletteTrans_AutoSet(fps->pfd, TRUE);
	PaletteFadeWorkAllocSet(fps->pfd, FADE_MAIN_BG, 0x200, HEAPID_FOOTPRINT);
	PaletteFadeWorkAllocSet(fps->pfd, FADE_SUB_BG, 0x200, HEAPID_FOOTPRINT);
	PaletteFadeWorkAllocSet(fps->pfd, FADE_MAIN_OBJ, 0x200-0x40, HEAPID_FOOTPRINT);	//通信アイコン-ローカライズ用
	PaletteFadeWorkAllocSet(fps->pfd, FADE_SUB_OBJ, 0x200, HEAPID_FOOTPRINT);
	PaletteTrans_AutoSet(fps->pfd, TRUE);
	
	fps->bgl = GF_BGL_BglIniAlloc(HEAPID_FOOTPRINT);

	initVramTransferManagerHeap(64, HEAPID_FOOTPRINT);

	sys_KeyRepeatSpeedSet( SYS_KEYREPEAT_SPEED_DEF, SYS_KEYREPEAT_WAIT_DEF );

	//VRAM割り当て設定
	FootPrint_VramBankSet(fps->bgl);

	// タッチパネルシステム初期化
	InitTPSystem();
	InitTPNoBuff(4);

	// ボタン用フォントを読み込み
	FontProc_LoadFont(FONT_BUTTON, HEAPID_FOOTPRINT);

	//メッセージマネージャ作成
	fps->wordset		 = WORDSET_Create(HEAPID_FOOTPRINT);
	fps->msgman       = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_wflby_footprint_dat, HEAPID_FOOTPRINT );

	//フォントOAMシステム作成
	fps->fontoam_sys = FONTOAM_SysInit(FOOT_FONTOAM_MAX, HEAPID_FOOTPRINT);

	//ハンドルを開ける(足跡など頻繁にグラフィックロードが行われるのでハンドル開けっ放しにする
	fps->handle_footprint = ArchiveDataHandleOpen( ARC_FOOTPRINT_GRA, HEAPID_FOOTPRINT );
	fps->handle_footmark = ArchiveDataHandleOpen(ARC_POKEFOOT_GRA, HEAPID_FOOTPRINT);

	// BGグラフィック転送
	BgGraphicSet( fps, fps->handle_footprint );
	//3Dモデル転送
	Model3DSet(fps, fps->handle_footprint);

	//カメラ作成
	Footprint_CameraInit(fps);

	// BMPWIN確保
	BmpWinInit( fps );
	
	fps->talk_strbuf = STRBUF_Create(256, HEAPID_FOOTPRINT);
	
	//アクターシステム作成
	fps->csp=CATS_AllocMemory(HEAPID_FOOTPRINT);
	CATS_SystemInit(fps->csp,&FootprintTcats,&FootprintCcmm,FOOTPRINT_OAM_PLTT_MAX);
	//通信アイコン用にキャラ＆パレット制限
	CLACT_U_WmIcon_SetReserveAreaCharManager(
		NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K);
	CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);
	fps->crp=CATS_ResourceCreate(fps->csp);
	CATS_ClactSetInit(fps->csp, fps->crp, FOOTPRINT_ACTOR_MAX);
	CATS_ResourceManagerInit(fps->csp,fps->crp,&FootprintResourceList);
	CLACT_U_SetSubSurfaceMatrix(CATS_EasyRenderGet(fps->csp), 0, FOOTPRINT_SUB_ACTOR_DISTANCE);

	// Wifi通信アイコン
    WirelessIconEasy();

	//スタンプシステム作成
	StampSys_Init(&fps->ssw, fps->arceus_flg);

	//常駐OBJ登録
	DefaultResourceSet_Main(fps, fps->handle_footprint);
	DefaultResourceSet_Sub(fps, fps->handle_footprint);
	DefaultActorSet_Main(fps);
	DefaultActorSet_Sub(fps);

	//自分のインクパレット初期設定
	MyInkPaletteSettings(fps);
	
	//ボタン「はい・いいえ」
	fps->yesno_button = TOUCH_SW_AllocWork(HEAPID_FOOTPRINT);
	
	// ワイプフェード開始
	WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, 
		WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_FOOTPRINT );

	// BGMをフェードアウト
	if( fps->parent_work->wflby_sys != NULL ){
		WFLBY_SYSTEM_SetBGMVolumeDown( fps->parent_work->wflby_sys, TRUE );
	}

	// 足跡ボードに入った音再生
//	Snd_SePlay( WFLBY_SND_FOOTIN );

	// BG面表示ON
	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG0, VISIBLE_ON );
	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG1, VISIBLE_ON );
	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );

	//メイン画面設定
	sys.disp3DSW = DISP_3D_TO_SUB;
	GF_Disp_DispSelect();
	GF_Disp_DispOn();
	GF_Disp_GX_VisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
	GF_Disp_GXS_VisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);

	MsgPrintSkipFlagSet(MSG_SKIP_ON);
	MsgPrintAutoFlagSet(MSG_AUTO_OFF);
	MsgPrintTouchPanelFlagSet(MSG_TP_OFF);

	fps->update_tcb = TCB_Add(Footprint_Update, fps, 60000);

	sys_VBlankFuncChange(VBlankFunc, fps);


	return PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   プロセス関数：メイン
 *
 * @param   proc		プロセスデータ
 * @param   seq			シーケンス
 *
 * @retval  処理状況
 */
//--------------------------------------------------------------
PROC_RESULT FootPrintProc_Main( PROC * proc, int * seq )
{
	FOOTPRINT_SYS * fps  = PROC_GetWork( proc );
	enum{
		SEQ_INIT,
		SEQ_IN_WIPE_WAIT,
		SEQ_MAIN,
		SEQ_EXIT_SELECT_INIT,
		SEQ_EXIT_SELECT_MSG_WAIT,
		SEQ_EXIT_SELECT,
		SEQ_TIMEUP_INIT,
		SEQ_TIMEUP_MSG_WAIT,
		SEQ_TIMEUP_WAIT,
		SEQ_OUT_INIT,
		SEQ_OUT_WAIT,
	};
	
	//スタンプシステム更新
	StampSys_Update(&fps->ssw, fps->camera, fps->game_status, fps->parent_work->board_type);

	switch(*seq){
	case SEQ_INIT:
		//通信
		fps->my_comm_status.ready = TRUE;	//全ての初期化が完了したので受信OK
		Footprint_Comm_Init(fps);
		{//入室情報送信
			FOOTPRINT_IN_PARAM in_para;
			Footprint_InParamCreate(fps, &in_para);
//			Footprint_Send_PlayerIn(&in_para);
		}
		(*seq)++;
		break;
	case SEQ_IN_WIPE_WAIT:
		if(WIPE_SYS_EndCheck() == TRUE){
			fps->game_status = FOOTPRINT_GAME_STATUS_NORMAL;
			(*seq)++;
		}
		break;
	case SEQ_MAIN:
		if(sys.tp_trg && sys.tp_y < 160){
			if(Footprint_InkGauge_Consume(fps, INK_GAUGE_CONSUME_STAMP) == TRUE){
				fps->my_stamp_param[fps->select_no].x = sys.tp_x;
				fps->my_stamp_param[fps->select_no].y = sys.tp_y;
			#ifdef PM_DEBUG
				if(DebugFoot.occ_seikaku){
					DebugFoot.backup_personal_rnd 
						= fps->my_stamp_param[fps->select_no].personal_rnd;
					if(sys.cont & PAD_BUTTON_B){
						fps->my_stamp_param[fps->select_no].personal_rnd 
							= DebugSeikakuTbl[gf_rand() % NELEMS(DebugSeikakuTbl)];
					}
					else{
						fps->my_stamp_param[fps->select_no].personal_rnd 
							= DebugSeikakuTbl[DebugFoot.move_type];
					}
				}
			#endif
				Footprint_StampAdd(
					fps, &fps->my_stamp_param[fps->select_no], fps->my_comm_status.user_id);
				Footprint_Send_Stamp(&fps->my_stamp_param[fps->select_no]);
			#ifdef PM_DEBUG
				if(DebugFoot.occ_seikaku){
					fps->my_stamp_param[fps->select_no].personal_rnd 
						= DebugFoot.backup_personal_rnd;
				}
			#endif
			}
			else{	//インクが足りない
				;		//何かSEとか鳴らすかも
			}
		}

		{
			int hit;
			
			hit = FootprintTool_InkPalTouchUpdate(fps->my_stamp_param, fps->select_no);
			if(hit < 6){
				fps->select_no = hit;
				Footprint_TouchEffAdd(fps, hit);
			}
			else if((*seq) == SEQ_MAIN && hit == FOOT_TOUCH_RET_EXIT){	//「やめる」を押した
				Snd_SePlay(FOOTPRINT_SE_TOUCH_EXIT);
				SoftFadePfd(fps->pfd, FADE_MAIN_BG, EXIT_BUTTON_COLOR_POS, 1, 
					EXIT_BUTTON_COLOR_EVY, EXIT_BUTTON_COLOR_CODE);
				SoftFadePfd(fps->pfd, FADE_MAIN_OBJ, fps->yameru_pal_pos * 16, 16, 
					EXIT_BUTTON_COLOR_EVY, EXIT_BUTTON_COLOR_CODE);
				*seq = SEQ_EXIT_SELECT_INIT;
			}
		}
		break;
	case SEQ_EXIT_SELECT_INIT:
		//会話ウィンドウ描画
		GF_BGL_BmpWinDataFill(&fps->talk_win, 0xf);
		BmpTalkWinWrite(&fps->talk_win, WINDOW_TRANS_ON, 
			WINCGX_TALKWIN_START, FOOT_MAINBG_TALKWIN_PAL);
		//メッセージ表示
		MSGMAN_GetString(fps->msgman, msg_footprint_exit_select, fps->talk_strbuf);
		fps->msg_index = GF_STR_PrintSimple(&fps->talk_win, FONT_TALK, 
			fps->talk_strbuf, 0, 0, 
			CONFIG_GetMsgPrintSpeed(SaveData_GetConfig(fps->sv)), NULL);
		(*seq)++;
		break;
	case SEQ_EXIT_SELECT_MSG_WAIT:
		if(GF_MSG_PrintEndCheck(fps->msg_index) == 0){
			//「はい・いいえ」ボタンを出す
			TOUCH_SW_PARAM tsp;
			
			tsp.p_bgl	  = fps->bgl;
			tsp.bg_frame  = FOOT_FRAME_WIN;
			tsp.char_offs = WINCGX_BUTTON_YESNO_START;
			tsp.pltt_offs = FOOT_MAINBG_BUTTON_YESNO_PAL;
			tsp.x		  = 25;
			tsp.y		  = 6;
			TOUCH_SW_Init(fps->yesno_button, &tsp);
			
			fps->yesno_button_use = TRUE;
			(*seq)++;
		}
		break;
	case SEQ_EXIT_SELECT:
		//VWaitタスクで転送されているので、Init時ではなくここでVRAMからコピーする
		PaletteWorkSet_VramCopy(fps->pfd, 
			FADE_MAIN_BG, FOOT_MAINBG_BUTTON_YESNO_PAL*16, 0x20*2);
		{
			u32 yesno_ret = TOUCH_SW_Main(fps->yesno_button);
			switch(yesno_ret){
			case TOUCH_SW_RET_YES:
				TOUCH_SW_Reset(fps->yesno_button);
				fps->yesno_button_use = 0;
				BmpTalkWinClear(&fps->talk_win, WINDOW_TRANS_ON);
				//SoftFadePfd(fps->pfd, FADE_MAIN_BG, EXIT_BUTTON_COLOR_POS, 1, 
				//	0, EXIT_BUTTON_COLOR_CODE);
				//SoftFadePfd(fps->pfd, FADE_MAIN_OBJ, fps->yameru_pal_pos * 16, 16, 
				//	0, EXIT_BUTTON_COLOR_CODE);
				
				fps->my_comm_status.ready = FALSE;	//終了処理に入るので受信受け取らない
				fps->game_status = FOOTPRINT_GAME_STATUS_FINISH;
				*seq = SEQ_OUT_INIT;
				break;
			case TOUCH_SW_RET_NO:
				TOUCH_SW_Reset(fps->yesno_button);
				fps->yesno_button_use = 0;
				BmpTalkWinClear(&fps->talk_win, WINDOW_TRANS_ON);
				SoftFadePfd(fps->pfd, FADE_MAIN_BG, EXIT_BUTTON_COLOR_POS, 1, 
					0, EXIT_BUTTON_COLOR_CODE);
				SoftFadePfd(fps->pfd, FADE_MAIN_OBJ, fps->yameru_pal_pos * 16, 16, 
					0, EXIT_BUTTON_COLOR_CODE);
				*seq = SEQ_MAIN;
				break;
			}
		}
		break;
	case SEQ_TIMEUP_INIT:
		//会話ウィンドウ描画
		GF_BGL_BmpWinDataFill(&fps->talk_win, 0xf);
		BmpTalkWinWrite(&fps->talk_win, WINDOW_TRANS_ON, 
			WINCGX_TALKWIN_START, FOOT_MAINBG_TALKWIN_PAL);
		//メッセージ表示
		MSGMAN_GetString(fps->msgman, msg_footprint_timeup, fps->talk_strbuf);
		fps->msg_index = GF_STR_PrintSimple(&fps->talk_win, FONT_TALK, 
			fps->talk_strbuf, 0, 0, 
			CONFIG_GetMsgPrintSpeed(SaveData_GetConfig(fps->sv)), NULL);
		(*seq)++;
		break;
	case SEQ_TIMEUP_MSG_WAIT:
		if(GF_MSG_PrintEndCheck(fps->msg_index) == 0){
			(*seq)++;
		}
		break;
	case SEQ_TIMEUP_WAIT:
		fps->timeup_wait++;
		if(fps->timeup_wait > FOOT_TIMEUP_WAIT){
			*seq = SEQ_OUT_INIT;
		}
		break;
	
	case SEQ_OUT_INIT:
		//退室情報送信
//		Footprint_Send_PlayerOut();

		if(WIPE_SYS_EndCheck() == FALSE){
			WIPE_SYS_ExeEnd();	//恐らくスペシャルエフェクトのフラッシュ中なので切る
		}

		// ワイプフェード開始
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, 
			WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_FOOTPRINT );
		(*seq)++;
		break;
	case SEQ_OUT_WAIT:
		if(WIPE_SYS_EndCheck() == TRUE){
			(*seq)++;
		}
		break;
	default:
		DWC_LOBBY_SUBCHAN_CleanMsgCmd();	//コマンドクリーン
		return PROC_RES_FINISH;
	}

	Footprint_TouchEffUpdate(fps);
	Footprint_InkGaugeUpdate(fps);

	if(fps->game_status == FOOTPRINT_GAME_STATUS_NORMAL){
		//参加者リスト更新処理
		fps->entry_list_update_timer++;
		if(fps->entry_list_update_timer > ENTRY_LIST_UPDATE_TIME){
			fps->entry_list_update_timer = 0;
			FootPrintTool_NameAllUpdate(fps);
		}
		
		//インクパレットタッチ判定処理
		if(((WFLBY_ERR_CheckError() == TRUE) 
				|| (WFLBY_SYSTEM_Event_GetEndCM(fps->parent_work->wflby_sys) == TRUE))
				&& (*seq) != SEQ_EXIT_SELECT_MSG_WAIT){	//メッセージ描画中は処理しない
			//WIFI広場の制限時間がきた
			if(fps->yesno_button_use == TRUE){
				//「はい・いいえ」ボタンが出ているなら消す
				TOUCH_SW_Reset(fps->yesno_button);
			}
			fps->my_comm_status.ready = FALSE;	//終了処理に入るので受信受け取らない
			fps->game_status = FOOTPRINT_GAME_STATUS_FINISH;
			if(WFLBY_ERR_CheckError() == TRUE){	//通信エラーによる終了
				(*seq) = SEQ_OUT_INIT;
			}
			else{	//タイムアップによる終了
				Snd_SePlay(FOOTPRINT_SE_TIMEUP);
				WFLBY_SYSTEM_APLFLAG_SetForceEnd( fps->parent_work->wflby_sys );	// 強制終了したことをロビーシステムに通知2.18 tomoya takahashi
				(*seq) = SEQ_TIMEUP_INIT;
			}
		}
	}

	//デバッグカメラ移動
	Debug_CameraMove(fps);
	
#ifdef PM_DEBUG
	if(sys.trg & PAD_BUTTON_START){
		DebugFoot.occ_seikaku ^= 1;
	}
	if(sys.trg & PAD_BUTTON_SELECT){
		DebugFoot.move_type++;
		if(DebugFoot.move_type >= NELEMS(DebugSeikakuTbl)){
			DebugFoot.move_type = 0;
		}
		OS_TPrintf("デバッグ move_type = %d\n", DebugFoot.move_type);
	}
	if(sys.trg & PAD_BUTTON_Y){
		DebugFoot.consume_zero ^= 1;
	}
#endif

	return PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   プロセス関数：終了
 *
 * @param   proc		プロセスデータ
 * @param   seq			シーケンス
 *
 * @retval  処理状況
 */
//--------------------------------------------------------------
PROC_RESULT FootPrintProc_End( PROC * proc, int * seq )
{
	FOOTPRINT_SYS * fps  = PROC_GetWork( proc );

	TCB_Delete(fps->update_tcb);

	DefaultActorDel_Main(fps);
	DefaultActorDel_Sub(fps);

	//ボタンの「はい・いいえ」削除
	TOUCH_SW_FreeWork(fps->yesno_button);
	
	//スタンプシステム破棄
	StampSys_Exit(&fps->ssw);

	STRBUF_Delete(fps->talk_strbuf);
	
	//フォント削除
	FontProc_UnloadFont(FONT_BUTTON);

	//フォントOAMシステム削除
	FONTOAM_SysDelete(fps->fontoam_sys);

	// メッセージマネージャー・ワードセットマネージャー解放
	MSGMAN_Delete( fps->msgman );
	WORDSET_Delete( fps->wordset );
	
	BmpWinDelete( fps );
	
	// BG_SYSTEM解放
	BgExit( fps->bgl );
	sys_FreeMemoryEz( fps->bgl );

	//アクターシステム削除
	CATS_ResourceDestructor_S(fps->csp,fps->crp);
	CATS_FreeMemory(fps->csp);

	//パレットフェードシステム削除
	PaletteFadeWorkAllocFree(fps->pfd, FADE_MAIN_BG);
	PaletteFadeWorkAllocFree(fps->pfd, FADE_SUB_BG);
	PaletteFadeWorkAllocFree(fps->pfd, FADE_MAIN_OBJ);
	PaletteFadeWorkAllocFree(fps->pfd, FADE_SUB_OBJ);
	PaletteFadeFree(fps->pfd);

	//3Dモデル解放
	Model3DDel(fps);
	//カメラ削除
	Footprint_CameraExit(fps);

	//simple_3DBGExit();
	Footprint_3D_Exit(fps->g3Dman);

	//ハンドル閉じる
	ArchiveDataHandleClose( fps->handle_footprint );
	ArchiveDataHandleClose( fps->handle_footmark );

	sys_VBlankFuncChange( NULL, NULL );		// VBlankセット
	sys_HBlankIntrStop();	//HBlank割り込み停止

	//Vram転送マネージャー削除
	DellVramTransferManager();

	StopTP();		//タッチパネルの終了

	MsgPrintSkipFlagSet(MSG_SKIP_OFF);
	MsgPrintAutoFlagSet(MSG_AUTO_OFF);
	MsgPrintTouchPanelFlagSet(MSG_TP_OFF);

	WirelessIconEasyEnd();

#ifdef PM_DEBUG
	if(fps->parent_work->wflby_sys == NULL){
		//デバッグ呼び出しの場合はparent_workの解放も行う
		sys_FreeMemoryEz(fps->parent_work);
	}
#endif

	PROC_FreeWork( proc );				// PROCワーク開放
	sys_DeleteHeap( HEAPID_FOOTPRINT );

	return PROC_RES_FINISH;
}


//==============================================================================
//	
//==============================================================================
static void Footprint_Update(TCB_PTR tcb, void *work)
{
	FOOTPRINT_SYS *fps = work;
	
	StampSys_HitCheck(&fps->ssw);

	Model3D_Update(fps);

	CATS_Draw(fps->crp);
	CATS_UpdateTransfer();
	GF_G3_RequestSwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);
	

#ifdef PM_DEBUG		//ポリゴンのラインズオーバーチェック
	if(G3X_IsLineBufferUnderflow() != 0){
		OS_TPrintf("--------------ラインズオーバー発生！！----------\n");
		//GF_ASSERT(0 && "ラインズオーバーが発生しました");
		G3X_ResetLineBufferUnderflow();
	}
	else if(G3X_GetRenderedLineCount() < 10){
		OS_TPrintf("========== ラインズオーバーが発生しそうです… Count = %d\n", 
			G3X_GetRenderedLineCount());
	}
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * VBlank関数
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankFunc( void * work )
{
	FOOTPRINT_SYS *fps = work;
	
	StampSys_VWaitUpdate(&fps->ssw, fps->game_status);

	// セルアクターVram転送マネージャー実行
	DoVramTransferManager();

	// レンダラ共有OAMマネージャVram転送
	CATS_RenderOamTrans();
	PaletteFadeTrans(fps->pfd);
	
	GF_BGL_VBlankFunc(fps->bgl);

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------
/**
 * @brief   Vramバンク設定を行う
 *
 * @param   bgl		BGLデータへのポインタ
 */
//--------------------------------------------------------------
static void FootPrint_VramBankSet(GF_BGL_INI *bgl)
{
	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
	
	//VRAM設定
	{
		GF_BGL_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_C,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット

			GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット

			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット

			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット

			GX_VRAM_TEX_01_AB,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_01_FG			// テクスチャパレットスロット
		};
		GF_Disp_SetBank( &vramSetTable );

		//VRAMクリア
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}

	// BG SYSTEM
	{
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}

	//メイン画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FOOT_FRAME_WIN	ウィンドウ面
			{
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
				FOOT_BGPRI_WIN, 0, 0, FALSE
			},
			///<FOOT_FRAME_PANEL	パネル面
			{
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
				FOOT_BGPRI_PANEL, 0, 0, FALSE
			},
			///<FOOT_FRAME_BG	背景
			{
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
				FOOT_BGPRI_BG, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, FOOT_FRAME_WIN, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, FOOT_FRAME_WIN );
		GF_BGL_ScrollSet(bgl, FOOT_FRAME_WIN, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FOOT_FRAME_WIN, GF_BGL_SCROLL_Y_SET, 0);

		GF_BGL_BGControlSet(bgl, FOOT_FRAME_PANEL, &TextBgCntDat[1], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, FOOT_FRAME_PANEL );
		GF_BGL_ScrollSet(bgl, FOOT_FRAME_PANEL, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FOOT_FRAME_PANEL, GF_BGL_SCROLL_Y_SET, 0);
		
		GF_BGL_BGControlSet(bgl, FOOT_FRAME_BG, &TextBgCntDat[2], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, FOOT_FRAME_BG );
		GF_BGL_ScrollSet(bgl, FOOT_FRAME_BG, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FOOT_FRAME_BG, GF_BGL_SCROLL_Y_SET, 0);

		//3D面
		G2_SetBG0Priority(FOOT_BGPRI_3D);
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	}
	//サブ画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FOOT_SUBFRAME_WIN	テキスト面
			{
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
				FOOT_SUBBGPRI_WIN, 0, 0, FALSE
			},
			///<FOOT_SUBFRAME_PLATE	プレート
			{
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
				FOOT_SUBBGPRI_PLATE, 0, 0, FALSE
			},
			///<FOOT_SUBFRAME_BG	背景
			{
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
				FOOT_SUBBGPRI_BG, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, FOOT_SUBFRAME_WIN, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, FOOT_SUBFRAME_WIN );
		GF_BGL_ScrollSet(bgl, FOOT_SUBFRAME_WIN, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FOOT_SUBFRAME_WIN, GF_BGL_SCROLL_Y_SET, 0);
		
		GF_BGL_BGControlSet(bgl, FOOT_SUBFRAME_PLATE, &TextBgCntDat[1], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, FOOT_SUBFRAME_PLATE );
		GF_BGL_ScrollSet(bgl, FOOT_SUBFRAME_PLATE, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FOOT_SUBFRAME_PLATE, GF_BGL_SCROLL_Y_SET, 0);

		GF_BGL_BGControlSet(bgl, FOOT_SUBFRAME_BG, &TextBgCntDat[2], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, FOOT_SUBFRAME_BG );
		GF_BGL_ScrollSet(bgl, FOOT_SUBFRAME_BG, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FOOT_SUBFRAME_BG, GF_BGL_SCROLL_Y_SET, 0);
	}

	GF_BGL_ClearCharSet( FOOT_SUBFRAME_WIN, 32, 0, HEAPID_FOOTPRINT );
}

//--------------------------------------------------------------------------------------------
/**
 * BG解放
 *
 * @param	ini		BGLデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgExit( GF_BGL_INI * ini )
{
	GF_BGL_BGControlExit( ini, FOOT_SUBFRAME_PLATE );
	GF_BGL_BGControlExit( ini, FOOT_SUBFRAME_WIN );
	GF_BGL_BGControlExit( ini, FOOT_SUBFRAME_BG );
	GF_BGL_BGControlExit( ini, FOOT_FRAME_BG );
	GF_BGL_BGControlExit( ini, FOOT_FRAME_PANEL );
	GF_BGL_BGControlExit( ini, FOOT_FRAME_WIN );

}

//--------------------------------------------------------------------------------------------
/**
 * グラフィックデータセット
 *
 * @param	fps		ポケモンリスト画面のワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgGraphicSet( FOOTPRINT_SYS * fps, ARCHANDLE* p_handle )
{
	GF_BGL_INI *bgl = fps->bgl;
	u16 *panel_scrn;
	
	//-- メイン画面 --//
	PaletteWorkSet_Arc(fps->pfd, ARC_FOOTPRINT_GRA, NARC_footprint_board_a_board_sita_NCLR, 
		HEAPID_FOOTPRINT, FADE_MAIN_BG, 0x200-0x40, 0);
	ArcUtil_HDL_BgCharSet(p_handle, NARC_footprint_board_a_board_sita_NCGR, fps->bgl, 
		FOOT_FRAME_PANEL, 0, 0, 0, HEAPID_FOOTPRINT);
	ArcUtil_HDL_ScrnSet(p_handle, NARC_footprint_board_a_board_sita_NSCR, fps->bgl, 
		FOOT_FRAME_PANEL, 0, 0, 0, HEAPID_FOOTPRINT);
	ArcUtil_HDL_ScrnSet(p_handle, NARC_footprint_board_a_board_sita_bg_NSCR, fps->bgl, 
		FOOT_FRAME_BG, 0, 0, 0, HEAPID_FOOTPRINT);

	//-- サブ画面 --//
	PaletteWorkSet_Arc(fps->pfd, ARC_FOOTPRINT_GRA, NARC_footprint_board_ashiato_board_NCLR, 
		HEAPID_FOOTPRINT, FADE_SUB_BG, 0, 0);
	if(fps->parent_work->board_type == FOOTPRINT_BOARD_TYPE_WHITE){
		PaletteWorkCopy(fps->pfd, FADE_SUB_BG, 16*1, FADE_SUB_BG, 16*0, 0x20);
	}
	ArcUtil_HDL_BgCharSet(p_handle, NARC_footprint_board_ashiato_board_NCGR, fps->bgl, 
		FOOT_SUBFRAME_PLATE, 0, 0, 0, HEAPID_FOOTPRINT);
	ArcUtil_HDL_ScrnSet(p_handle, NARC_footprint_board_ashiato_board_NSCR, fps->bgl, 
		FOOT_SUBFRAME_PLATE, 0, 0, 0, HEAPID_FOOTPRINT);
	ArcUtil_HDL_ScrnSet(p_handle, NARC_footprint_board_ashiato_board_bg_NSCR, fps->bgl, 
		FOOT_SUBFRAME_BG, 0, 0, 0, HEAPID_FOOTPRINT);
	panel_scrn = GF_BGL_ScreenAdrsGet(fps->bgl, FOOT_SUBFRAME_PLATE);
	MI_CpuCopy16(panel_scrn, fps->namelist_scrn, NAMELIST_SCRN_SIZE);
	MI_CpuClear16(panel_scrn, NAMELIST_SCRN_SIZE);
	
	{
		int win_type;
	
		win_type = CONFIG_GetWindowType(SaveData_GetConfig(fps->sv));
		
		// 会話ウィンドウパレット転送
		PaletteWorkSet_Arc(fps->pfd, ARC_WINFRAME, TalkWinPalArcGet(win_type), HEAPID_FOOTPRINT, 
			FADE_MAIN_BG, 0x20, FOOT_MAINBG_TALKWIN_PAL * 16);
		// 会話ウインドウグラフィック転送
		TalkWinGraphicSet(fps->bgl, FOOT_FRAME_WIN, WINCGX_TALKWIN_START, 
			FOOT_MAINBG_TALKWIN_PAL,  win_type, HEAPID_FOOTPRINT);

		//システムフォントパレット転送
		PaletteWorkSet_Arc(fps->pfd, ARC_FONT, NARC_font_system_ncrl, HEAPID_FOOTPRINT, 
			FADE_MAIN_BG, 0x20, FOOT_MAINBG_TALKFONT_PAL * 16);
		if(fps->parent_work->board_type == FOOTPRINT_BOARD_TYPE_WHITE){
			PaletteWorkSet_Arc(fps->pfd, ARC_FONT, NARC_font_system_ncrl, HEAPID_FOOTPRINT, 
				FADE_SUB_BG, 0x20, FOOT_SUBBG_TALKFONT_PAL * 16);
		}
		else{
			PaletteWorkSet_Arc(fps->pfd, ARC_FOOTPRINT_GRA, 
				NARC_footprint_board_a_board_font_s_NCLR, HEAPID_FOOTPRINT, 
				FADE_SUB_BG, 0x20, FOOT_SUBBG_TALKFONT_PAL * 16);
		}
	}

	GF_BGL_LoadScreenV_Req(fps->bgl, FOOT_SUBFRAME_PLATE);
}

//--------------------------------------------------------------
/**
 * @brief   メイン画面の常駐OBJ用リソースのセット
 *
 * @param   fps		
 */
//--------------------------------------------------------------
static void DefaultResourceSet_Main(FOOTPRINT_SYS *fps, ARCHANDLE *hdl_main)
{
	int i, pal_pos;
	u16 color_code;
	
	//-- メイン画面OBJ常駐パレット --//
	CATS_LoadResourcePlttWorkArcH(fps->pfd, FADE_MAIN_OBJ, fps->csp, fps->crp, 
		hdl_main, NARC_footprint_board_a_board_eff_NCLR, 0, FOOTPRINT_COMMON_PAL_NUM, 
		NNS_G2D_VRAM_TYPE_2DMAIN, PLTTID_OBJ_COMMON);

	//-- インク(インクの下地も共通) --//
	CATS_LoadResourceCharArcH(fps->csp, fps->crp, hdl_main, 
		NARC_footprint_board_ashiato_gage_NCGR, 0, NNS_G2D_VRAM_TYPE_2DMAIN, CHARID_INK);
	CATS_LoadResourceCellArcH(fps->csp, fps->crp, hdl_main, 
		NARC_footprint_board_ashiato_gage_NCER, 0, CELLID_INK);
	CATS_LoadResourceCellAnmArcH(fps->csp, fps->crp, hdl_main, 
		NARC_footprint_board_ashiato_gage_NANR, 0, CELLANMID_INK);

	//-- インクの上に配置する足跡 --//
	for(i = 0; i < POKEMON_TEMOTI_MAX; i++){
		CATS_LoadResourceCharArcH(fps->csp, fps->crp, hdl_main, 
			NARC_footprint_board_wifi_mark_NCGR, 0, 
			NNS_G2D_VRAM_TYPE_2DMAIN, CHARID_INK_FOOT_0 + i);
	}
	CATS_LoadResourceCellArcH(fps->csp, fps->crp, hdl_main, 
		NARC_footprint_board_wifi_mark_NCER, 0, CELLID_INK_FOOT);
	CATS_LoadResourceCellAnmArcH(fps->csp, fps->crp, hdl_main, 
		NARC_footprint_board_wifi_mark_NANR, 0, CELLANMID_INK_FOOT);
	//インクの上に配置する足跡のパレット(データはダミー。領域だけ確保)
	//ダミーグラフィックは7、足跡データは4番のカラーで書かれているので、
	//一本確保して、足跡用に割り当ててしまう。全てのカラーを足跡の色で埋める
	pal_pos = CATS_LoadResourcePlttWorkArcH(fps->pfd, FADE_MAIN_OBJ, fps->csp, fps->crp, 
		hdl_main, NARC_footprint_board_a_board_eff_NCLR, 0, 1, 
		NNS_G2D_VRAM_TYPE_2DMAIN, PLTTID_OBJ_INK_FOOT);
	if(fps->parent_work->board_type == FOOTPRINT_BOARD_TYPE_WHITE){
		color_code = INKPAL_FOOT_COLOR_CODE_WHITE;
	}
	else{
		color_code = INKPAL_FOOT_COLOR_CODE_BLACK;
	}
	PaletteWork_Clear(fps->pfd, FADE_MAIN_OBJ, FADEBUF_ALL, color_code, 
		pal_pos * 16, pal_pos * 16 + 16);
	
	//-- タッチエフェクト --//
	CATS_LoadResourceCharArcH(fps->csp, fps->crp, hdl_main, 
		NARC_footprint_board_a_board_eff_NCGR, 0, NNS_G2D_VRAM_TYPE_2DMAIN, CHARID_TOUCH_EFF);
	CATS_LoadResourceCellArcH(fps->csp, fps->crp, hdl_main, 
		NARC_footprint_board_a_board_eff_NCER, 0, CELLID_TOUCH_EFF);
	CATS_LoadResourceCellAnmArcH(fps->csp, fps->crp, hdl_main, 
		NARC_footprint_board_a_board_eff_NANR, 0, CELLANMID_TOUCH_EFF);
		
	//-- FONTOAM --//
	fps->yameru_pal_pos = CATS_LoadResourcePlttWorkArc(
		fps->pfd, FADE_MAIN_OBJ, fps->csp, fps->crp, 
		ARC_FOOTPRINT_GRA, NARC_footprint_board_a_board_font_b_NCLR, 0, 1, 
		NNS_G2D_VRAM_TYPE_2DMAIN, PLTTID_OBJ_FONTOAM);
}

//--------------------------------------------------------------
/**
 * @brief   メイン画面の常駐アクター登録
 *
 * @param   fps		
 */
//--------------------------------------------------------------
static void DefaultActorSet_Main(FOOTPRINT_SYS *fps)
{
	int i;
	TCATS_OBJECT_ADD_PARAM_S head;
	
	//-- インク --//
	head = InkObjParam;
	for(i = 0; i < POKEMON_TEMOTI_MAX; i++){
		head.x = INK_POS_START_X + INK_POS_SPACE_X * i;
		head.y = INK_POS_Y;
		fps->cap_ink[i] = CATS_ObjectAdd_S(fps->csp, fps->crp, &head);
		CATS_ObjectAnimeSeqSetCap(fps->cap_ink[i], i);
		CATS_ObjectUpdate(fps->cap_ink[i]->act);
	}

	//-- インクの下地 --//
	head = InkFoundationObjParam;
	for(i = 0; i < POKEMON_TEMOTI_MAX; i++){
		head.x = INK_FOUNDATION_POS_START_X + INK_FOUNDATION_POS_SPACE_X * i;
		head.y = INK_FOUNDATION_POS_Y;
		fps->cap_ink_foundation[i] = CATS_ObjectAdd_S(fps->csp, fps->crp, &head);
		CATS_ObjectAnimeSeqSetCap(fps->cap_ink_foundation[i], i);
		CATS_ObjectUpdate(fps->cap_ink_foundation[i]->act);
	}
	
	//-- インクの上に配置する足跡 --//
	head = InkFootObjParam;
	for(i = 0; i < POKEMON_TEMOTI_MAX; i++){
		head.x = INK_FOOT_POS_START_X + INK_FOOT_POS_SPACE_X * i;
		head.y = INK_FOOT_POS_Y;
		head.id[CLACT_U_CHAR_RES] = CHARID_INK_FOOT_0 + i;
		fps->cap_ink_foot[i] = CATS_ObjectAdd_S(fps->csp, fps->crp, &head);
		CATS_ObjectUpdate(fps->cap_ink_foot[i]->act);
	}

	//-- 「やめる」FONTOAM --//
	{
		STRBUF *str_ptr;
		
		str_ptr = MSGMAN_AllocString(fps->msgman, msg_footprint_exit);
		
		Sub_FontOamCreate(fps, &fps->fontoam_exit, str_ptr, FONT_BUTTON,
			GF_PRINTCOLOR_MAKE(1,2,3), 0, 
			PLTTID_OBJ_FONTOAM, 0x1c * 8, 176, FONTOAM_CENTER);
		FONTOAM_SetDrawFlag(fps->fontoam_exit.fontoam, TRUE);
		
		STRBUF_Delete(str_ptr);
	}
}

//--------------------------------------------------------------
/**
 * @brief   メイン画面の常駐アクターを削除する
 *
 * @param   fps		
 */
//--------------------------------------------------------------
static void DefaultActorDel_Main(FOOTPRINT_SYS *fps)
{
	int i;
	
	//-- インク --//
	for(i = 0; i < POKEMON_TEMOTI_MAX; i++){
		CATS_ActorPointerDelete_S(fps->cap_ink[i]);
	}
	
	//-- インクの下地 --//
	for(i = 0; i < POKEMON_TEMOTI_MAX; i++){
		CATS_ActorPointerDelete_S(fps->cap_ink_foundation[i]);
	}

	//-- インクの上に配置する足跡 --//
	for(i = 0; i < POKEMON_TEMOTI_MAX; i++){
		CATS_ActorPointerDelete_S(fps->cap_ink_foot[i]);
	}

	//-- FONTOAM --//
	Sub_FontOamDelete(&fps->fontoam_exit);
}

//--------------------------------------------------------------
/**
 * @brief   サブ画面の常駐OBJ用リソースのセット
 *
 * @param   fps		
 */
//--------------------------------------------------------------
static void DefaultResourceSet_Sub(FOOTPRINT_SYS *fps, ARCHANDLE *hdl_main)
{
	int i;
	
	//-- サブ画面OBJ常駐パレット --//
	CATS_LoadResourcePlttWorkArcH(fps->pfd, FADE_SUB_OBJ, fps->csp, fps->crp, 
		hdl_main, NARC_footprint_board_ashiato_frame_NCLR, 0, FOOTPRINT_SUB_COMMON_PAL_NUM, 
		NNS_G2D_VRAM_TYPE_2DSUB, PLTTID_SUB_OBJ_COMMON);

	//-- 名前を囲むフレーム --//
	CATS_LoadResourceCharArcH(fps->csp, fps->crp, hdl_main, 
		NARC_footprint_board_ashiato_frame_NCGR, 0, NNS_G2D_VRAM_TYPE_2DSUB, 
		CHARID_SUB_NAME_FRAME);
	CATS_LoadResourceCellArcH(fps->csp, fps->crp, hdl_main, 
		NARC_footprint_board_ashiato_frame_NCER, 0, CELLID_SUB_NAME_FRAME);
	CATS_LoadResourceCellAnmArcH(fps->csp, fps->crp, hdl_main, 
		NARC_footprint_board_ashiato_frame_NANR, 0, CELLANMID_SUB_NAME_FRAME);
		
	//-- 名前の横の足跡 --//
	for(i = 0; i < FOOTPRINT_ENTRY_MAX; i++){
		CATS_LoadResourceCharArcH(fps->csp, fps->crp, hdl_main, 
			NARC_footprint_board_foot_dummy_NCGR, 0, NNS_G2D_VRAM_TYPE_2DSUB, 
			CHARID_SUB_NAME_FOOT_0 + i);
	}
	CATS_LoadResourceCellArcH(fps->csp, fps->crp, hdl_main, 
		NARC_footprint_board_foot_dummy_NCER, 0, CELLID_SUB_NAME_FOOT);
	CATS_LoadResourceCellAnmArcH(fps->csp, fps->crp, hdl_main, 
		NARC_footprint_board_foot_dummy_NANR, 0, CELLANMID_SUB_NAME_FOOT);
}

//--------------------------------------------------------------
/**
 * @brief   サブ画面の常駐アクター登録
 *
 * @param   fps		
 */
//--------------------------------------------------------------
static void DefaultActorSet_Sub(FOOTPRINT_SYS *fps)
{
	int i;
	TCATS_OBJECT_ADD_PARAM_S head;
	
	//-- 名前を囲む枠 --//
	fps->cap_name_frame = CATS_ObjectAdd_S(fps->csp, fps->crp, &NameFrameObjParam);
	CATS_ObjectUpdate(fps->cap_name_frame->act);
	CATS_ObjectEnableCap(fps->cap_name_frame, CATS_ENABLE_FALSE);	//最初は非表示

	//-- 名前の横の足跡 --//
	head = NameFootObjParam;
	for(i = 0; i < FOOTPRINT_ENTRY_MAX; i++){
		head.id[CLACT_U_CHAR_RES] = CHARID_SUB_NAME_FOOT_0 + i;
		fps->cap_name_foot[i] = CATS_ObjectAdd_S(fps->csp, fps->crp, &head);
		CATS_ObjectPosSetCap_SubSurface(fps->cap_name_foot[i], 
			Sub_FootmarkPos[i][0], Sub_FootmarkPos[i][1], FOOTPRINT_SUB_ACTOR_DISTANCE);
		CATS_ObjectUpdate(fps->cap_name_foot[i]->act);
		CATS_ObjectEnableCap(fps->cap_name_foot[i], CATS_ENABLE_FALSE);	//最初は非表示
	}
}

//--------------------------------------------------------------
/**
 * @brief   サブ画面の常駐アクターを削除する
 *
 * @param   fps		
 */
//--------------------------------------------------------------
static void DefaultActorDel_Sub(FOOTPRINT_SYS *fps)
{
	int i;
	
	//-- 名前を囲む枠 --//
	CATS_ActorPointerDelete_S(fps->cap_name_frame);
	
	//-- 名前の横の足跡 --//
	for(i = 0; i < FOOTPRINT_ENTRY_MAX; i++){
		CATS_ActorPointerDelete_S(fps->cap_name_foot[i]);
	}
}

//--------------------------------------------------------------
/**
 * @brief   自分のインクパレットの初期設定を行う
 *
 * @param  fps
 */
//--------------------------------------------------------------
static void MyInkPaletteSettings(FOOTPRINT_SYS *fps)
{
	int i, y;
	u16 *def_pal, *trans_pal;
	int first_hit = 0;
	
	def_pal = PaletteWorkDefaultWorkGet(fps->pfd, FADE_MAIN_OBJ);
	trans_pal = PaletteWorkTransWorkGet(fps->pfd, FADE_MAIN_OBJ);
	
	for(i = 0; i < POKEMON_TEMOTI_MAX; i++){
		if(fps->my_stamp_param[i].monsno == 0 || fps->my_stamp_param[i].monsno > MONSNO_END){
			CATS_ObjectEnableCap(fps->cap_ink[i], CATS_ENABLE_FALSE);
			CATS_ObjectEnableCap(fps->cap_ink_foundation[i], CATS_ENABLE_FALSE);
			CATS_ObjectEnableCap(fps->cap_ink_foot[i], CATS_ENABLE_FALSE);

			//BGのインクの枠を消す
			for(y = 0; y < SCRN_INK_POS_SIZE_Y; y++){
				GF_BGL_ScrFill(fps->bgl, FOOT_FRAME_PANEL, MyInkPaletteEraseScrnCode[y], 
					SCRN_INK_POS_START_X + SCRN_INK_POS_SIZE_X * i, SCRN_INK_POS_Y + y,
					SCRN_INK_POS_SIZE_X, 1, GF_BGL_SCRWRT_PALIN);
			}
		}
		else{
			//足跡のキャラを書き換える
			OBJFootCharRewrite(fps->my_stamp_param[i].monsno, fps->my_stamp_param[i].form_no, 
				fps->cap_ink_foot[i], fps->handle_footprint,
				fps->handle_footmark, NNS_G2D_VRAM_TYPE_2DMAIN, fps->arceus_flg);
			
			//インクのカラーを変更する
			def_pal[PALOFS_INK * 16 + COLOR_NO_INK_START + i] = fps->my_stamp_param[i].color;
			trans_pal[PALOFS_INK * 16 + COLOR_NO_INK_START + i] = fps->my_stamp_param[i].color;
			//インクの下地を変更する(黒っぽくする)
			SoftFade(&def_pal[PALOFS_INK * 16 + COLOR_NO_INK_START + i],
				&def_pal[PALOFS_INK_FOUNDATION * 16 + COLOR_NO_INK_START + i],
				1, INK_FOUNDATION_EVY, INK_FOUNDATION_COLOR);
			trans_pal[PALOFS_INK_FOUNDATION * 16 + COLOR_NO_INK_START + i]
				= def_pal[PALOFS_INK_FOUNDATION * 16 + COLOR_NO_INK_START + i];
			
			if(first_hit == 0){
				first_hit++;
				fps->select_no = i;
			}
		}
	}
	Footprint_SelectInkPaletteFade(fps, 0);
	GF_BGL_LoadScreenV_Req(fps->bgl, FOOT_FRAME_PANEL);
}

//--------------------------------------------------------------
/**
 * @brief   OBJの足跡マークを書き換える
 *
 * @param   monsno			ポケモン番号
 * @param   cap				書き換え対象のアクターへのポインタ
 * @param   hdl_mark		足跡グラフィックのハンドル
 * @param   NNS_G2D_VRAM_TYPE	NNS_G2D_VRAM_TYPE_2DMAIN or NNS_G2D_VRAM_TYPE_2DSUB
 * @param   arceus_flg		アルセウス公開フラグ(TRUE:公開OK)
 *
 * @retval  TRUE:書き換えた
 * @retval  FALSE:書き換えなかった
 */
//--------------------------------------------------------------
static BOOL OBJFootCharRewrite(int monsno, int form_no, CATS_ACT_PTR cap, ARCHANDLE *hdl_main, ARCHANDLE *hdl_mark, NNS_G2D_VRAM_TYPE vram_type, BOOL arceus_flg)
{
	void *obj_vram;
	NNSG2dImageProxy * image;
	void *pSrc;
	NNSG2dCharacterData *pChar;
	u8 *read_up, *read_bottom;
	
	if(monsno == 0 || monsno > MONSNO_END){
		return FALSE;
	}
	
	OS_TPrintf("書き換えmonsno = %d\n", monsno);
	if(FootprintTool_FootDispCheck(monsno, form_no, arceus_flg) == TRUE){
		pSrc = ArcUtil_HDL_Load(hdl_mark, POKEFOOT_ARC_CHAR_DMMY + monsno, //足跡
			TRUE, HEAPID_FOOTPRINT, ALLOC_BOTTOM);
		NNS_G2dGetUnpackedCharacterData(pSrc, &pChar);
		DC_FlushRange(pChar->pRawData, 0x20 * 8);
		read_up = &((u8*)pChar->pRawData)[0x20 * 4];	//128Kマッピングで空白が入っているので
		read_bottom = pChar->pRawData;
	}
	else{	//足跡が無いのでWifiマーク
		pSrc = ArcUtil_HDL_Load(hdl_main, NARC_footprint_board_wifi_mark_NCGR, //WIFIマーク
			FALSE, HEAPID_FOOTPRINT, ALLOC_BOTTOM);
		NNS_G2dGetUnpackedCharacterData(pSrc, &pChar);
		DC_FlushRange(pChar->pRawData, 0x20 * 4);
		read_up = pChar->pRawData;
		read_bottom = &((u8*)pChar->pRawData)[0x20 * 2];
	}
	
	if(vram_type == NNS_G2D_VRAM_TYPE_2DMAIN){
		obj_vram = G2_GetOBJCharPtr();
	}
	else{
		obj_vram = G2S_GetOBJCharPtr();
	}
	image = CLACT_ImageProxyGet(cap->act);
	
	//書き込み
	MI_CpuCopy16(read_up, (void*)((u32)obj_vram
		+ image->vramLocation.baseAddrOfVram[vram_type]), 
		0x20 * 2);
	MI_CpuCopy16(read_bottom, (void*)((u32)obj_vram + 0x20*2
		+ image->vramLocation.baseAddrOfVram[vram_type]), 
		0x20 * 2);
	
	sys_FreeMemoryEz(pSrc);
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   カメラ作成
 *
 * @param   fps		ゲームワークへのポインタ
 */
//--------------------------------------------------------------
static void Footprint_CameraInit(FOOTPRINT_SYS *fps)
{
	VecFx32	target = { FOOTPRINT_CAMERA_TX, FOOTPRINT_CAMERA_TY, FOOTPRINT_CAMERA_TZ };

	fps->camera = GFC_AllocCamera( HEAPID_FOOTPRINT );

	GFC_InitCameraTDA(&target, FOOTPRINT_CAMERA_DISTANCE, &FootprintCameraAngle,
						FOOTPRINT_CAMERA_PERSPWAY, FOOTPRINT_CAMERA_MODE, FALSE, fps->camera);

	GFC_SetCameraClip( FOOTPRINT_CAMERA_NEAR, FOOTPRINT_CAMERA_FAR, fps->camera);
	
	GFC_AttachCamera(fps->camera);
	
	{//設定したカメラ位置での、ワールド空間の範囲を取得する
		u16 persp_way;
		fx32 distance, aspect;
		fx32 width, height;
		
		persp_way = GFC_GetCameraPerspWay(fps->camera);
		distance = GFC_GetCameraDistance(fps->camera);
		aspect = FX32_ONE * 4 / 3;
		GetPerspectiveScreenSize(persp_way, distance, aspect, &width, &height);
		OS_TPrintf("width = %d(%x), height = %d(%x)\n", width, width, height, height);
		
		fps->world_width = width;
		fps->world_height = height;
	}
}

//--------------------------------------------------------------
/**
 * @brief   カメラ解放
 *
 * @param   fps		ゲームワークへのポインタ
 */
//--------------------------------------------------------------
static void Footprint_CameraExit(FOOTPRINT_SYS *fps)
{
	GFC_FreeCamera(fps->camera);
}

//--------------------------------------------------------------
/**
 * @brief   3Dモデルセット
 *
 * @param   fps		
 */
//--------------------------------------------------------------
static void Model3DSet( FOOTPRINT_SYS * fps, ARCHANDLE* p_handle )
{
	int data_id;
	
	if(fps->parent_work->board_type == FOOTPRINT_BOARD_TYPE_WHITE){
		data_id = NARC_footprint_board_a_board2_nsbmd;
	}
	else{
		data_id = NARC_footprint_board_a_board1_nsbmd;
	}
	
    //モデルデータ読み込み
	D3DOBJ_MdlLoadH(&fps->board.mdl, p_handle, data_id, HEAPID_FOOTPRINT);

    //レンダーオブジェクトに登録
    D3DOBJ_Init( &fps->board.obj, &fps->board.mdl );

    //座標設定
    D3DOBJ_SetMatrix( &fps->board.obj, BOARD_X, BOARD_Y, BOARD_Z);
    D3DOBJ_SetScale(&fps->board.obj, BOARD_SCALE, BOARD_SCALE, BOARD_SCALE);
    D3DOBJ_SetDraw( &fps->board.obj, TRUE );
}

//--------------------------------------------------------------
/**
 * @brief   3Dモデル削除
 *
 * @param   fps		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void Model3DDel(FOOTPRINT_SYS *fps)
{
    // 全リソース破棄
    D3DOBJ_MdlDelete( &fps->board.mdl ); 
}

//--------------------------------------------------------------
/**
 * @brief   3Dモデル更新
 *
 * @param   fps		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void Model3D_Update(FOOTPRINT_SYS *fps)
{
	VecFx32 scale_vec, trans;
	MtxFx33 rot;
	
	scale_vec.x = FX32_ONE;
	scale_vec.y = FX32_ONE;
	scale_vec.z = FX32_ONE;
	
	trans.x = 0;
	trans.y = 0;
	trans.z = 0;
	
	MTX_Identity33(&rot);

	//３Ｄ描画開始
	GF_G3X_Reset();
	
	GFC_AttachCamera(fps->camera);
	GFC_SetCameraView(FOOTPRINT_CAMERA_MODE, fps->camera); //正射影設定
	GFC_CameraLookAt();

	// ライトとアンビエント
	NNS_G3dGlbLightVector( 0, 0, -FX32_ONE, 0 );
	NNS_G3dGlbLightColor( 0, GX_RGB( 28,28,28 ) );
	NNS_G3dGlbMaterialColorDiffAmb( GX_RGB( 31,31,31 ), GX_RGB( 31,31,31 ), FALSE );
	NNS_G3dGlbMaterialColorSpecEmi( GX_RGB( 31,31,31 ), GX_RGB( 31,31,31 ), FALSE );
	
	// 位置設定
	NNS_G3dGlbSetBaseTrans(&trans);
	// 角度設定
	NNS_G3dGlbSetBaseRot(&rot);
	// スケール設定
	NNS_G3dGlbSetBaseScale(&scale_vec);

//	NNS_G3dGlbFlush();
	
	// 描画
	NNS_G3dGePushMtx();
	{
		//ボード
		D3DOBJ_Draw( &fps->board.obj );
		//スタンプ
		StampSys_ObjDraw(&fps->ssw);
	}
	NNS_G3dGePopMtx(1);
}

//------------------------------------------------------------------
/**
 * BMPWIN処理（文字パネルにフォント描画）
 *
 * @param   fps		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void BmpWinInit( FOOTPRINT_SYS *fps )
{
	int i;
	
	//-- メイン画面 --//
	GF_BGL_BmpWinAdd(fps->bgl, &fps->talk_win, FOOT_FRAME_WIN,
		2, 1, 27, 4, FOOT_MAINBG_TALKFONT_PAL, WINCGX_MESSAGE_START);
	GF_BGL_BmpWinDataFill(&fps->talk_win, 0xf);
	
	//-- サブ画面 --//
	//参加者の名前表示
	for(i = 0; i < FOOTPRINT_BMPWIN_NAME_MAX; i++){
		GF_BGL_BmpWinAdd(fps->bgl, &fps->name_win[i], FOOT_SUBFRAME_WIN,
			NameBmpwinPos[i][0], NameBmpwinPos[i][1], 
			WINCGX_SUB_NAME_SIZE_X, WINCGX_SUB_NAME_SIZE_Y,
			FOOT_SUBBG_TALKFONT_PAL, 
			WINCGX_SUB_NAME_START + (WINCGX_SUB_NAME_SIZE_X * WINCGX_SUB_NAME_SIZE_Y) * i);
		GF_BGL_BmpWinDataFill(&fps->name_win[i], 0x00);
	}
}

//------------------------------------------------------------------
/**
 * $brief   確保したBMPWINを解放
 *
 * @param   fps		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void BmpWinDelete( FOOTPRINT_SYS *fps )
{
	int i;
	
	//-- メイン画面 --//
	GF_BGL_BmpWinDel(&fps->talk_win);
	
	//-- サブ画面 --//
	for(i = 0; i < FOOTPRINT_BMPWIN_NAME_MAX; i++){
		GF_BGL_BmpWinDel(&fps->name_win[i]);
	}
}

//--------------------------------------------------------------
/**
 * @brief   足跡ボード用3DBG初期化関数
 * 
 * @param   ヒープID
 */
//--------------------------------------------------------------
static GF_G3DMAN * Footprint_3D_Init(int heap_id)
{
	GF_G3DMAN *g3Dman;
	
	g3Dman = GF_G3DMAN_Init(heap_id, GF_G3DMAN_LNK, GF_G3DTEX_256K, 
		GF_G3DMAN_LNK, GF_G3DPLT_32K, FootprintSimpleSetUp);
	return g3Dman;
}

static void FootprintSimpleSetUp(void)
{
	// ３Ｄ使用面の設定(表示＆プライオリティー)
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
    G2_SetBG0Priority(1);

	// 各種描画モードの設定(シェード＆アンチエイリアス＆半透明)
    G3X_SetShading( GX_SHADING_TOON );
    G3X_AntiAlias( TRUE );
	G3X_AlphaTest( FALSE, 0 );	// アルファテスト　　オフ
	G3X_AlphaBlend( TRUE );		// アルファブレンド　オン
	G3X_EdgeMarking( FALSE );
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	// クリアカラーの設定
    G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog

	// ビューポートの設定
    G3_ViewPort(0, 0, 255, 191);
}

//--------------------------------------------------------------
/**
 * @brief   足跡ボード用3DBG終了処理
 *
 * @param   g3Dman		
 */
//--------------------------------------------------------------
static void Footprint_3D_Exit(GF_G3DMAN *g3Dman)
{
	GF_G3D_Exit(g3Dman);
}

//--------------------------------------------------------------
/**
 * @brief   自分の通信ステータスをセットする
 *
 * @param   fps		
 */
//--------------------------------------------------------------
static void Footprint_MyCommStatusSet(FOOTPRINT_SYS *fps)
{
	FOOTPRINT_MY_COMM_STATUS *my_comm;
	
	my_comm = &fps->my_comm_status;
	
	my_comm->user_id = DWC_LOBBY_GetMyUserID();
}

//--------------------------------------------------------------
/**
 * @brief   FOOTPRINT_IN_PARAMの中身を作成する
 *
 * @param   fps			
 * @param   in_para		データセット先
 */
//--------------------------------------------------------------
static void Footprint_InParamCreate(FOOTPRINT_SYS_PTR fps, FOOTPRINT_IN_PARAM *in_para)
{
	STAMP_PARAM *stamp;
	
	MI_CpuClear8(in_para, sizeof(FOOTPRINT_IN_PARAM));
	stamp = &in_para->stamp;
	
	//最初に名前の横に表示させる足跡の状態をセットする
	//先頭にいるポケモンの足跡をセット。
	//タマゴの場合は飛ばす
	stamp->monsno = 150;
	stamp->color = 0x001f;
	stamp->personal_rnd = 0x0101;
	stamp->form_no = 0;
}

//--------------------------------------------------------------
/**
 * @brief   自分の通信ステータスを取得する
 * @param   fps		
 * @retval  通信ステータスへのポインタ
 */
//--------------------------------------------------------------
FOOTPRINT_MY_COMM_STATUS * Footprint_MyCommStatusGet(FOOTPRINT_SYS_PTR fps)
{
	return &fps->my_comm_status;
}

//--------------------------------------------------------------
/**
 * @brief   手持ちポケモンをスタンプパラメータにセットする
 *
 * @param   board_type		ボードの種類(FOOTPRINT_BOARD_TYPE_???)
 * @param   sv				セーブデータへのポインタ
 * @param   stamp_array		代入先(スタンプパラメータの配列へのポインタ)
 */
//--------------------------------------------------------------
static void Footprint_Temoti_to_StampParam(int board_type, SAVEDATA * sv, STAMP_PARAM *stamp_array)
{
	POKEPARTY *party;
	int poke_max;
	POKEMON_PARAM *pp;
	int i;
	
	MI_CpuClear8(stamp_array, sizeof(STAMP_PARAM) * POKEMON_TEMOTI_MAX);
	
	party = SaveData_GetTemotiPokemon(sv);
	poke_max = PokeParty_GetPokeCount(party);
	for(i = 0; i < poke_max; i++){
		pp = PokeParty_GetMemberPointer(party, i);
		stamp_array[i].monsno = PokeParaGet(pp, ID_PARA_monsno_egg, NULL);
		stamp_array[i].personal_rnd = PokeParaGet(pp, ID_PARA_personal_rnd, NULL);
		stamp_array[i].form_no = PokeParaGet(pp, ID_PARA_form_no, NULL);
		stamp_array[i].color 
			= FootprintTool_StampColorGet(board_type, PokeParaGet(pp, ID_PARA_id_no, NULL));
	#ifdef PM_DEBUG
		if(sys.cont & PAD_BUTTON_B){	//ランダムで色を変える
			stamp_array[i].color = FootprintTool_StampColorGet(board_type, gf_rand());
		}
	#endif
	}
}

//--------------------------------------------------------------
/**
 * @brief   カメラ設置位置での、ワールド空間座標範囲を取得する
 *
 * @param   fps		
 * @param   width		設置されているカメラに表示されているワールド座標の幅
 * @param   height		設置されているカメラに表示されているワールド座標の高さ
 */
//--------------------------------------------------------------
void Footprint_WorldWidthHeightGet(FOOTPRINT_SYS_PTR fps, fx32 *width, fx32 *height)
{
	*width = fps->world_width;
	*height = fps->world_height;
}

//--------------------------------------------------------------
/**
 * @brief   スタンプを生成する
 *
 * @param   fps			
 * @param   param		スタンプパラメータ
 *
 * @retval  TRUE:成功。　FALSE:失敗
 */
//--------------------------------------------------------------
BOOL Footprint_StampAdd(FOOTPRINT_SYS_PTR fps, const STAMP_PARAM *param, s32 user_id)
{
	int ret;
	u32 user_index;
	
	ret = StampSys_Add(fps, &fps->ssw, param, 
		fps->handle_footprint, fps->handle_footmark, fps->arceus_flg);
	if(ret == TRUE){
		//上画面の足跡マークを変更する
		user_index = DWC_LOBBY_SUBCHAN_GetUserIDIdx(user_id);
		if(user_index != DWC_LOBBY_USERIDTBL_IDX_NONE){
			if(fps->name_foot_monsno[user_index] != param->monsno
					|| fps->name_foot_color[user_index] != param->color){
				fps->name_foot_monsno[user_index] = param->monsno;
				fps->name_foot_color[user_index] = param->color;
				OBJFootCharRewrite(param->monsno, param->form_no, fps->cap_name_foot[user_index], 
					fps->handle_footprint, fps->handle_footmark, 
					NNS_G2D_VRAM_TYPE_2DSUB, fps->arceus_flg);
				CATS_ObjectEnableCap(fps->cap_name_foot[user_index], CATS_ENABLE_TRUE);
				//パレットも変える
				PaletteWork_Clear(fps->pfd, FADE_SUB_BG, FADEBUF_ALL, param->color, 
					SUBBG_LIST_COLOR_START + user_index, SUBBG_LIST_COLOR_START + user_index + 1);
			}
		}
		
		Snd_SePlay(FOOTPRINT_SE_STAMP);
	}
	
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   サブチャンネルのリストを参照して名前リストを更新する
 *
 * @param   msgman		
 * @param   wordset		
 * @param   win[]		
 * @param   wflby_sys		
 * @param   entry_userid[]		
 *
 * @retval  FOOTPRINT_NAME_UPDATE_STATUS_???
 * 			(ENTRYとLEAVE_ROOMの両方が発生した場合はENTRYを優先して返します)
 */
//--------------------------------------------------------------
static FOOTPRINT_NAME_UPDATE_STATUS FootPrintTool_NameAllUpdate(FOOTPRINT_SYS *fps)
{
	DWC_LOBBY_CHANNEL_USERID chan_user;
	int i, entry_num, out_num, player_max;
	
	entry_num = 0;
	out_num = 0;
	
	DWC_LOBBY_SUBCHAN_GetUserIDTbl(&chan_user);	// サブチャンネル内ユーザID列挙取得
	for(i = 0; i < FOOTPRINT_ENTRY_MAX; i++){
		OS_TPrintf("chan_user.cp_tbl[%d] = %d\n", i, chan_user.cp_tbl[i]);
		if(fps->entry_userid[i] != chan_user.cp_tbl[i]){
			//変更があったものだけ再描画
			if(chan_user.cp_tbl[i] != DWC_LOBBY_INVALID_USER_ID){
				OS_TPrintf("新規ユーザー名描画 index = %d\n", i);
				FootPrintTool_NameDraw(fps->msgman, fps->wordset, fps->name_win, 
					fps->parent_work->wflby_sys, chan_user.cp_tbl[i]);
				//名前リストのスクリーン描画
				{
					u16 *panel_scrn;
					int x, y;
					
					panel_scrn = GF_BGL_ScreenAdrsGet(fps->bgl, FOOT_SUBFRAME_PLATE);
					for(y = Sub_ListScrnRange[i][1]; y < Sub_ListScrnRange[i][1] + Sub_ListScrnRange[i][3]; y++){
						MI_CpuCopy16(&fps->namelist_scrn[y*32 + Sub_ListScrnRange[i][0]], 
							&panel_scrn[y*32 + Sub_ListScrnRange[i][0]], 
							Sub_ListScrnRange[i][2] * 2);
					}
					GF_BGL_LoadScreenV_Req(fps->bgl, FOOT_SUBFRAME_PLATE);
				}

				entry_num++;
			}
			else{
				OS_TPrintf("ユーザー名消去　index = %d番\n", i);
				FootPrintTool_NameErase(fps->name_win, i);
				fps->name_foot_monsno[i] = 0;
				fps->name_foot_color[i] = 0;
				CATS_ObjectEnableCap(fps->cap_name_foot[i], CATS_ENABLE_FALSE);	//足跡非表示
				GF_BGL_ScrFill(fps->bgl, FOOT_SUBFRAME_PLATE, 0, 
					Sub_ListScrnRange[i][0], Sub_ListScrnRange[i][1],
					Sub_ListScrnRange[i][2], Sub_ListScrnRange[i][3], GF_BGL_SCRWRT_PALNL);
				GF_BGL_LoadScreenV_Req(fps->bgl, FOOT_SUBFRAME_PLATE);
				out_num++;
			}
			
			//自分を表す枠の位置をセット
			if(chan_user.cp_tbl[i] == fps->my_comm_status.user_id){
				CATS_ObjectPosSetCap_SubSurface(fps->cap_name_frame,
					Sub_NameFramePos[i][0], Sub_NameFramePos[i][1], FOOTPRINT_SUB_ACTOR_DISTANCE);
				CATS_ObjectEnableCap(fps->cap_name_frame, CATS_ENABLE_TRUE);
			}
		}
		fps->entry_userid[i] = chan_user.cp_tbl[i];
//		OS_TPrintf("参加者(%d)のuserid = %d\n", i, chan_user.cp_tbl[i]);
	}

	//参加人数をカウント
	player_max = 0;
	for(i = 0; i < FOOTPRINT_ENTRY_MAX; i++){
		if(fps->entry_userid[i] != DWC_LOBBY_INVALID_USER_ID){
			player_max++;
		}
	}
	fps->ssw.player_max = player_max;
	
	if(entry_num > 0){
		Snd_SePlay(FOOTPRINT_SE_ENTRY);
		return FOOTPRINT_NAME_UPDATE_STATUS_ENTRY;
	}
	else if(out_num > 0){
		Snd_SePlay(FOOTPRINT_SE_LEAVE_ROOM);
		return FOOTPRINT_NAME_UPDATE_STATUS_LEAVE_ROOM;
	}
	return FOOTPRINT_NAME_UPDATE_STATUS_NULL;
}

//--------------------------------------------------------------
/**
 * @brief   参加者の名前描画
 *
 * @param   fps				
 * @param   user_index		ユーザーid
 */
//--------------------------------------------------------------
void Footprint_NameWrite(FOOTPRINT_SYS_PTR fps, s32 user_id)
{
	FootPrintTool_NameDraw(fps->msgman, fps->wordset, fps->name_win, 
		fps->parent_work->wflby_sys, user_id);
}

//--------------------------------------------------------------
/**
 * @brief   参加者の名前を消去する
 *
 * @param   fps		
 * @param   user_index		ユーザーindex
 */
//--------------------------------------------------------------
void Footprint_NameErase(FOOTPRINT_SYS_PTR fps, u32 user_index)
{
	FootPrintTool_NameErase(fps->name_win, user_index);
}

//--------------------------------------------------------------
/**
 * @brief   タッチエフェクトを生成する
 *
 * @param   fps			
 * @param   hit_pos		インクパレットのヒット位置
 */
//--------------------------------------------------------------
static void Footprint_TouchEffAdd(FOOTPRINT_SYS_PTR fps, int hit_pos)
{
	TCATS_OBJECT_ADD_PARAM_S head;
	int i;
	
	//タッチエフェクトアクター生成
	head = TouchEffObjParam;
	for(i = 0; i < TOUCH_EFF_MAX; i++){
		if(fps->cap_touch_eff[i] == NULL){
			head.x = INK_FOOT_POS_START_X + INK_FOOT_POS_SPACE_X * hit_pos;
			head.y = INK_FOOT_POS_Y;
			fps->cap_touch_eff[i] = CATS_ObjectAdd_S(fps->csp, fps->crp, &head);
//			CATS_ObjectUpdate(fps->cap_touch_eff[i]->act);
			break;
		}
	}
	
	Footprint_SelectInkPaletteFade(fps, hit_pos);

	Snd_SePlay(FOOTPRINT_SE_TOUCH_INK);
}

//--------------------------------------------------------------
/**
 * @brief   現在選択中のインクパレットの色を落とす
 *
 * @param   fps			
 * @param   hit_pos		インクパレットのヒット位置
 */
//--------------------------------------------------------------
static void Footprint_SelectInkPaletteFade(FOOTPRINT_SYS_PTR fps, int hit_pos)
{
	u16 *def_pal, *trans_pal;

	//押した所のインクパレットの色を落とす
	def_pal = PaletteWorkDefaultWorkGet(fps->pfd, FADE_MAIN_OBJ);
	trans_pal = PaletteWorkTransWorkGet(fps->pfd, FADE_MAIN_OBJ);
	//パレットをまず元通りにする
	MI_CpuCopy16(&def_pal[PALOFS_INK * 16 + COLOR_NO_INK_START], 
		&trans_pal[PALOFS_INK * 16 + COLOR_NO_INK_START], POKEMON_TEMOTI_MAX * 2);
	//対象位置のパレットを暗くする
	SoftFade(&def_pal[PALOFS_INK * 16 + COLOR_NO_INK_START + hit_pos],
		&trans_pal[PALOFS_INK * 16 + COLOR_NO_INK_START + hit_pos], 
		1, TOUCH_EFF_EVY, TOUCH_EFF_COLOR);
}

//--------------------------------------------------------------
/**
 * @brief   タッチエフェクト更新処理
 *
 * @param   fps		
 */
//--------------------------------------------------------------
static void Footprint_TouchEffUpdate(FOOTPRINT_SYS_PTR fps)
{
	int i;
	
	for(i = 0; i < TOUCH_EFF_MAX; i++){
		if(fps->cap_touch_eff[i] != NULL){
			if(CATS_ObjectAnimeActiveCheckCap(fps->cap_touch_eff[i]) == FALSE){
				CATS_ActorPointerDelete_S(fps->cap_touch_eff[i]);
				fps->cap_touch_eff[i] = NULL;
			}
			else{
				CATS_ObjectUpdate(fps->cap_touch_eff[i]->act);
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   フォントOAMを作成する
 *
 * @param   aci			BIシステムワークへのポインタ
 * @param   font_actor	生成したフォントOAM関連のワーク代入先
 * @param   str			文字列
 * @param   font_type	フォントタイプ(FONT_SYSTEM等)
 * @param   color		フォントカラー構成
 * @param   pal_offset	パレット番号オフセット
 * @param   pal_id		登録開始パレットID
 * @param   x			座標X
 * @param   y			座標Y
 * @param   pos_center  FONTOAM_LEFT(X左端座標) or FONTOAM_CENTER(X中心座標)
 */
//--------------------------------------------------------------
static void Sub_FontOamCreate(FOOTPRINT_SYS_PTR fps, FONT_ACTOR *font_actor, const STRBUF *str, 
	FONT_TYPE font_type, GF_PRINTCOLOR color, int pal_offset, int pal_id, 
	int x, int y, int pos_center)
{
	FONTOAM_INIT finit;
	GF_BGL_BMPWIN bmpwin;
	CHAR_MANAGER_ALLOCDATA cma;
	int vram_size;
	FONTOAM_OBJ_PTR fontoam;
	GF_BGL_INI *bgl;
	CATS_RES_PTR crp;
	int font_len, char_len;
	
	GF_ASSERT(font_actor->fontoam == NULL);
	
	bgl = fps->bgl;
	crp = fps->crp;
	
	//文字列のドット幅から、使用するキャラ数を算出する
	FontLenGet(str, font_type, &font_len, &char_len);

	//BMP作成
	GF_BGL_BmpWinInit(&bmpwin);
	GF_BGL_BmpWinObjAdd(bgl, &bmpwin, char_len, 16 / 8, 0, 0);
	GF_STR_PrintExpand(&bmpwin, font_type, str, 0, 0, MSG_NO_PUT, color, 
		0, 0, NULL);
//	GF_STR_PrintColor(&bmpwin, font_type, str, 0, 0, MSG_NO_PUT, color, NULL );

	vram_size = FONTOAM_NeedCharSize(&bmpwin, NNS_G2D_VRAM_TYPE_2DMAIN,  HEAPID_FOOTPRINT);
	CharVramAreaAlloc(vram_size, CHARM_CONT_AREACONT, NNS_G2D_VRAM_TYPE_2DMAIN, &cma);
	
	//座標位置修正
	if(pos_center == FONTOAM_CENTER){
		x -= font_len / 2;
	}
//	y += ACTIN_SUB_ACTOR_DISTANCE_INTEGER - 8;
	y -= 8;
	
	finit.fontoam_sys = fps->fontoam_sys;
	finit.bmp = &bmpwin;
	finit.clact_set = CATS_GetClactSetPtr(crp);
	finit.pltt = CATS_PlttProxy(crp, pal_id);
	finit.parent = NULL;
	finit.char_ofs = cma.alloc_ofs;
	finit.x = x;
	finit.y = y;
	finit.bg_pri = ACTBGPRI_EXIT_FONT;
	finit.soft_pri = SOFTPRI_EXIT_FONT;
	finit.draw_area = NNS_G2D_VRAM_TYPE_2DMAIN;
	finit.heap = HEAPID_FOOTPRINT;
	
	fontoam = FONTOAM_Init(&finit);
//	FONTOAM_SetPaletteOffset(fontoam, pal_offset);
	FONTOAM_SetPaletteOffsetAddTransPlttNo(fontoam, pal_offset);
	FONTOAM_SetMat(fontoam, x, y);
	
	//解放処理
	GF_BGL_BmpWinDel(&bmpwin);
	
	font_actor->fontoam = fontoam;
	font_actor->cma = cma;
	font_actor->font_len = font_len;
}

//--------------------------------------------------------------
/**
 * @brief   生成されているフォントOAMを全て削除をする
 * @param   fps		BIシステムワークへのポインタ
 */
//--------------------------------------------------------------
static void Sub_FontOamDelete(FONT_ACTOR *font_actor)
{
	FONTOAM_Delete(font_actor->fontoam);
	CharVramAreaFree(&font_actor->cma);
}

//--------------------------------------------------------------
/**
 * @brief   文字列の長さを取得する
 *
 * @param   str				文字列へのポインタ
 * @param   font_type		フォントタイプ
 * @param   ret_dot_len		ドット幅代入先
 * @param   ret_char_len	キャラ幅代入先
 */
//--------------------------------------------------------------
static void FontLenGet(const STRBUF *str, FONT_TYPE font_type, int *ret_dot_len, int *ret_char_len)
{
	int dot_len, char_len;
	
	//文字列のドット幅から、使用するキャラ数を算出する
	dot_len = FontProc_GetPrintStrWidth(font_type, str, 0);
	char_len = dot_len / 8;
	if(FX_ModS32(dot_len, 8) != 0){
		char_len++;
	}
	
	*ret_dot_len = dot_len;
	*ret_char_len = char_len;
}

//--------------------------------------------------------------
/**
 * @brief   インクゲージ更新処理
 *
 * @param   fps		
 */
//--------------------------------------------------------------
static void Footprint_InkGaugeUpdate(FOOTPRINT_SYS_PTR fps)
{
	int offset_y, i;
	
	if(fps->ink_now == fps->ink_calc){	//インクを回復
		fps->ink_now += INK_GAUGE_RESTORE_NUM;
		if(fps->ink_now > INK_GAUGE_TANK_MAX){
			fps->ink_now = INK_GAUGE_TANK_MAX;
		}
		fps->ink_calc = fps->ink_now;
	}
	else{	//インクを減らし中
		fps->ink_calc -= fps->ink_sub;
		if(fps->ink_calc < fps->ink_now){
			fps->ink_calc = fps->ink_now;
		}
	}
	
	//インクの量からゲージの現在地を決定
	offset_y = INK_GAUGE_LEN - (fps->ink_calc >> 8);
	for(i = 0; i < POKEMON_TEMOTI_MAX; i++){
		CATS_ObjectPosSetCap_SubSurface(fps->cap_ink[i],
			INK_POS_START_X + INK_POS_SPACE_X * i, 
			INK_POS_Y + offset_y, FOOTPRINT_SUB_ACTOR_DISTANCE);
	}
}

//--------------------------------------------------------------
/**
 * @brief   インクゲージを消費する
 *
 * @param   fps				
 * @param   consume_num		消費量
 *
 * @retval  TRUE:インクが足りた(消費OK)
 * @retval  FALSE:インクが足りない(消費NG)
 */
//--------------------------------------------------------------
static BOOL Footprint_InkGauge_Consume(FOOTPRINT_SYS_PTR fps, int consume_num)
{
#ifdef PM_DEBUG
	if(DebugFoot.consume_zero){
		consume_num = 0;
	}
#endif

	if(fps->ink_now < consume_num){
		return FALSE;	//インクが足りない
	}
	
	if(fps->ink_calc < fps->ink_now){
		GF_ASSERT(0);	//ink_calcがnowよりも下の値になる事はありえない
		fps->ink_calc = fps->ink_now;
	}
	fps->ink_now -= consume_num;
	
	fps->ink_sub = (fps->ink_calc - fps->ink_now) / INK_GAUGE_SUB_FRAME;
	
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   デバッグ機能：カメラ移動
 *
 * @param   fps		
 */
//--------------------------------------------------------------
static void Debug_CameraMove(FOOTPRINT_SYS *fps)
{
#ifdef PM_DEBUG
	VecFx32 move = {0,0,0};
	fx32 value = FX32_ONE;
	int add_angle = 64;
	CAMERA_ANGLE angle = {0,0,0,0};
	int mode = 0;
	enum{
		MODE_SHIFT,			//平行移動
		MODE_DISTANCE,		//距離
		MODE_ANGLE_REV,		//公転
	};
	
	if((sys.cont & PAD_BUTTON_L) && (sys.cont & PAD_BUTTON_R)){
		mode = MODE_DISTANCE;
	}
	else if(sys.cont & PAD_BUTTON_L){
		mode = MODE_SHIFT;
	}
	else if(sys.cont & PAD_BUTTON_R){
		mode = MODE_ANGLE_REV;
	}
	else{
		return;
	}
	
	switch(mode){
	case MODE_SHIFT:
		if(sys.cont & PAD_KEY_LEFT){
			move.x -= value;
		}
		if(sys.cont & PAD_KEY_RIGHT){
			move.x += value;
		}
		if(sys.cont & PAD_KEY_UP){
			move.y += value;
		}
		if(sys.cont & PAD_KEY_DOWN){
			move.y -= value;
		}
		if(sys.cont & PAD_BUTTON_X){
			move.z -= value;
		}
		if(sys.cont & PAD_BUTTON_B){
			move.z += value;
		}
		GFC_ShiftCamera(&move, fps->camera);
		move = GFC_GetCameraPos(fps->camera);
		OS_TPrintf("カメラ位置 x=%d(16進:%x), y=%d(16進:%x), z=%d(16進:%x)\n", move.x, move.x, move.y, move.y, move.z, move.z);
		break;
	
	case MODE_ANGLE_REV:
		if(sys.cont & PAD_KEY_LEFT){
			angle.y -= add_angle;
		}
		if(sys.cont & PAD_KEY_RIGHT){
			angle.y += add_angle;
		}
		if(sys.cont & PAD_KEY_UP){
			angle.x += add_angle;
		}
		if(sys.cont & PAD_KEY_DOWN){
			angle.x -= add_angle;
		}
		if(sys.cont & PAD_BUTTON_X){
			angle.z -= add_angle;
		}
		if(sys.cont & PAD_BUTTON_B){
			angle.z += add_angle;
		}
		GFC_AddCameraAngleRev(&angle, fps->camera);
		angle = GFC_GetCameraAngle(fps->camera);
		OS_TPrintf("カメラアングル　x=%d, y=%d, z=%d\n", angle.x, angle.y, angle.z);
		break;
	case MODE_DISTANCE:
		if(sys.cont & PAD_KEY_UP){
			GFC_AddCameraDistance(FX32_ONE, fps->camera);
		}
		if(sys.cont & PAD_KEY_DOWN){
			GFC_AddCameraDistance(-FX32_ONE, fps->camera);
		}
		OS_TPrintf("カメラ距離＝%d(16進:%x)\n", GFC_GetCameraDistance(fps->camera), GFC_GetCameraDistance(fps->camera));
		break;
	}
#endif
}

