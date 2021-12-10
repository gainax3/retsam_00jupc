//==============================================================================================
/**
 * @file	scratch_sys.h
 * @brief	「スクラッチ」システムヘッダー
 * @author	Satoshi Nohara
 * @date	07.12.11
 */
//==============================================================================================
#ifndef _SCRATCH_SYS_H_
#define _SCRATCH_SYS_H_

#include "common.h"
#include "system/procsys.h"


//==============================================================================================
//
//	スクラッチワークへの不完全型ポインタ
//
//==============================================================================================
typedef	struct _SCRATCH_WORK SCRATCH_WORK;


//==============================================================================================
//
//	定義
//
//==============================================================================================
//パレット定義
#define BS_TALKWIN_PAL			(10)				//会話ウィンドウ使用パレットナンバー
#define BS_MENU_PAL				(11)				//メニューウィンドウ使用パレットナンバー
#define BS_MSGFONT_PAL			(12)				//MSGフォント使用パレットナンバー
#define BS_FONT_PAL				(13)				//SYSTEMフォント使用パレットナンバー
#define SCRATCH_TOUCH_SUB_PAL	(10)				//下画面「はい、いいえ」使用パレットナンバー

#define SCRATCH_VANISH_ON		(0)					//非表示
#define SCRATCH_VANISH_OFF		(1)					//表示

//リソースマネージャーオブジェクトナンバー
enum{
	SCRATCH_RES_OBJ_U_POKE = 0,						//ポケモン
	SCRATCH_RES_OBJ_D_POKE,							//ポケモン
#if 1
	SCRATCH_RES_OBJ_D_CARD,							//カード
	SCRATCH_RES_OBJ_D_BUTTON,						//ボタン
	SCRATCH_RES_OBJ_D_ATARI,						//当たりウィンドウ
#else
	//カードと切り替え
	SCRATCH_RES_OBJ_D_CARD,							//カード
	SCRATCH_RES_OBJ_D_BUTTON=SCRATCH_RES_OBJ_D_CARD,//ボタン(カードと切り替え)
#endif
	SCRATCH_RES_OBJ_MAX,							//最大数
};

//管理ID
enum{
	SCRATCH_ID_OBJ_U_POKE = 0,						//ポケモン
	SCRATCH_ID_OBJ_D_POKE,							//ポケモン
#if 1
	SCRATCH_ID_OBJ_D_CARD,							//カード
	SCRATCH_ID_OBJ_D_BUTTON,						//ボタン
	SCRATCH_ID_OBJ_D_ATARI,							//当たりウィンドウ
#else
	//カードと切り替え
	SCRATCH_ID_OBJ_D_CARD,							//カード
	SCRATCH_ID_OBJ_D_BUTTON=SCRATCH_ID_OBJ_D_CARD,	//ボタン(カードと切り替え)
#endif
	SCRATCH_ID_OBJ_MAX,
};

#define SCRATCH_U_POKE_MAX		(4)					//上表示ポケモン数
#define SCRATCH_ATARI_MAX		(4)					//当たりの数
#define SCRATCH_D_POKE_MAX		(9)					//下表示ポケモン数
#define SCRATCH_D_WAKU_MAX		(9)					//下表示ワク数
#define SCRATCH_D_CARD_MAX		(4)					//下表示カード数

//上画面：ポケモン12
//下画面：ポケモン9+カード4+ボタン1+ウィン+1+銀箔ワク9+当たりポケモン4+当たりウィン1
#define SCRATCH_CLACT_OBJ_MAX	(40)				//セルアクターに設定する最大アクター数

//アニメナンバー
enum{
	ANM_CARD_BLUE = 0,								//青
	ANM_CARD_RED,									//赤
	ANM_CARD_YELLOW,								//黄色
	ANM_CARD_GREEN,									//緑
};

//アニメナンバー
enum{
	ANM_POKE_REE = 0,								//リーシャン
	ANM_POKE_TYE,									//チェリンボ
	ANM_POKE_TAMA,									//タマザラシ
	ANM_POKE_GOKU,									//ゴクリン
	ANM_POKE_META,									//メタモン
};

//アニメナンバー
enum{
	ANM_BUTTON_YAMERU = 0,							//やめるボタン
	ANM_BUTTON_NEXT,								//つぎのカードへボタン
	ANM_GIN_WAKU,									//銀箔のワク
	ANM_CENTER_WIN,									//真ん中に表示するメッセージのウィンドウ
};

#define SCRATCH_FRAME_U_MSG		(GF_BGL_FRAME0_S)	//メッセージ
#define SCRATCH_FRAME_TYPE		(GF_BGL_FRAME1_S)	//未使用
#define SCRATCH_FRAME_U_YAKU	(GF_BGL_FRAME2_S)	//役背景
#define SCRATCH_FRAME_U_BG		(GF_BGL_FRAME3_S)	//4枚のカードを選ぶ時の背景
#define SCRATCH_FRAME_D_TOUCH	(GF_BGL_FRAME0_M)	//削り面
#define SCRATCH_FRAME_D_SUB		(GF_BGL_FRAME1_M)	//カード背景
#define SCRATCH_FRAME_D_MSG		(GF_BGL_FRAME2_M)	//メッセージ
//#define SCRATCH_FRAME_D_ATARI	(GF_BGL_FRAME3_M)	//当たりウィンドウ

#define SCRATCH_U_MASK_MSG		(GX_PLANEMASK_BG0)	//メッセージ
#define SCRATCH_U_MASK_TYPE		(GX_PLANEMASK_BG1)	//未使用
#define SCRATCH_U_MASK_YAKU		(GX_PLANEMASK_BG2)	//役背景
#define SCRATCH_U_MASK_BG		(GX_PLANEMASK_BG3)	//4枚のカードを選ぶ時の背景
#define SCRATCH_D_MASK_TOUCH	(GX_PLANEMASK_BG0)	//削り面
#define SCRATCH_D_MASK_SUB		(GX_PLANEMASK_BG1)	//カード背景
#define SCRATCH_D_MASK_MSG		(GX_PLANEMASK_BG2)	//メッセージ
//#define SCRATCH_D_MASK_ATARI	(GX_PLANEMASK_BG3)	//当たりウィンドウ

typedef struct{
	s16  x;
	s16  y;
}SCRATCH_POS;

//CLACTで定義しているセルが大きすぎてサブ画面に影響がでてしまうので離してみる
//oekaki.c,guru2_receipt.c参考
//#define SCRATCH_SUB_ACTOR_DISTANCE 	( 256 * FX32_ONE )
#define SCRATCH_SUB_ACTOR_DISTANCE 	( 512 * FX32_ONE )		//拡大させるのでさらに離す

#define SCRATCH_CARD_NUM		(3)						//スクラッチのカードの数

//OBJとのプライオリティ
enum{
	SCRATCH_OBJ_PRI_H = 0,
	SCRATCH_OBJ_PRI_N = 10,
	SCRATCH_OBJ_PRI_L = 20,
};

//BGとのプライオリティ
enum{
	SCRATCH_OBJ_BG_PRI_H = 0,
	SCRATCH_OBJ_BG_PRI_N = 1,
	SCRATCH_OBJ_BG_PRI_L = 2,
};

//ワクのパレットリソース数
#define WAKU_PAL_NUM			(8)


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern const PROC_DATA ScratchProcData;				//プロセス定義データ
extern PROC_RESULT ScratchProc_Init( PROC * proc, int * seq );
extern PROC_RESULT ScratchProc_Main( PROC * proc, int * seq );
extern PROC_RESULT ScratchProc_End( PROC * proc, int * seq );


#endif //_SCRATCH_SYS_H_


