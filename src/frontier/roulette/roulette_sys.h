//==============================================================================================
/**
 * @file	roulette_sys.h
 * @brief	「バトルルーレット」システムヘッダー
 * @author	Satoshi Nohara
 * @date	07.09.06
 */
//==============================================================================================
#ifndef _ROULETTE_SYS_H_
#define _ROULETTE_SYS_H_

#include "common.h"
#include "system/procsys.h"


//==============================================================================================
//
//	タクトワークへの不完全型ポインタ
//
//==============================================================================================
typedef	struct _ROULETTE_WORK ROULETTE_WORK;


//==============================================================================================
//
//	定義
//
//==============================================================================================
//パレット定義
#define BR_TALKWIN_PAL			(10)				//会話ウィンドウ使用パレットナンバー
#define BR_MENU_PAL				(11)				//メニューウィンドウ使用パレットナンバー
#define BR_MSGFONT_PAL			(12)				//MSGフォント使用パレットナンバー
#define BR_FONT_PAL				(13)				//SYSTEMフォント使用パレットナンバー

#define ROULETTE_VANISH_ON		(0)					//非表示
#define ROULETTE_VANISH_OFF		(1)					//表示

//パネルの数
#define ROULETTE_PANEL_H_MAX	(4)
#define ROULETTE_PANEL_V_MAX	(4)
#define ROULETTE_PANEL_MAX		(ROULETTE_PANEL_H_MAX * ROULETTE_PANEL_V_MAX)

//リソースマネージャーオブジェクトナンバー
enum{
	ROULETTE_RES_OBJ_BUTTON = 0,					//ボタン
	ROULETTE_RES_OBJ_CSR,							//カーソル
	ROULETTE_RES_OBJ_ITEMKEEP,						//アイテム持たせているアイコン
	ROULETTE_RES_OBJ_ICON1,							//アイコン1
	ROULETTE_RES_OBJ_ICON2,							//アイコン2
	ROULETTE_RES_OBJ_ICON3,							//アイコン3
	ROULETTE_RES_OBJ_ICON4,							//アイコン4
	ROULETTE_RES_OBJ_ICON5,							//アイコン5
	ROULETTE_RES_OBJ_ICON6,							//アイコン6
	ROULETTE_RES_OBJ_ICON7,							//アイコン7
	ROULETTE_RES_OBJ_ICON8,							//アイコン8
	ROULETTE_RES_OBJ_MAX,							//最大数(リソースファイルは1つ、あとはアイコン)
};

//管理CHAR_ID
enum{
	ROULETTE_ID_CHAR_BUTTON = 0,					//ボタン
	ROULETTE_ID_CHAR_CSR,							//カーソル
	ROULETTE_ID_CHAR_ITEMKEEP,						//アイテム持たせているアイコン
	ROULETTE_ID_CHAR_ICON1,							//アイコン1
	ROULETTE_ID_CHAR_ICON2,							//アイコン2
	ROULETTE_ID_CHAR_ICON3,							//アイコン3
	ROULETTE_ID_CHAR_ICON4,							//アイコン4
	ROULETTE_ID_CHAR_ICON5,							//アイコン5
	ROULETTE_ID_CHAR_ICON6,							//アイコン6
	ROULETTE_ID_CHAR_ICON7,							//アイコン7
	ROULETTE_ID_CHAR_ICON8,							//アイコン8
	ROULETTE_ID_CHAR_MAX,							//
};

//管理PLTT_ID
enum{
	ROULETTE_ID_PLTT_BUTTON = 0,					//ボタン
	ROULETTE_ID_PLTT_CSR,							//カーソル
	ROULETTE_ID_PLTT_ITEMKEEP,						//アイテム持たせているアイコン
	ROULETTE_ID_PLTT_ICON,							//アイコン
	ROULETTE_ID_PLTT_MAX,							//
};

//管理CELL_ID
enum{
	ROULETTE_ID_CELL_BUTTON = 0,					//ボタン
	ROULETTE_ID_CELL_CSR,							//カーソル
	ROULETTE_ID_CELL_ITEMKEEP,						//アイテム持たせているアイコン
	ROULETTE_ID_CELL_ICON,							//アイコン
	ROULETTE_ID_CELL_MAX,							//
};

//管理CELLANM_ID
enum{
	ROULETTE_ID_CELLANM_BUTTON = 0,					//ボタン
	ROULETTE_ID_CELLANM_CSR,						//カーソル
	ROULETTE_ID_CELLANM_ITEMKEEP,					//アイテム持たせているアイコン
	ROULETTE_ID_CELLANM_ICON,						//アイコン
	ROULETTE_ID_CELLANM_MAX,						//
};

//管理ID
enum{
	ROULETTE_ID_OBJ_BUTTON = 0,						//ボタン
	ROULETTE_ID_OBJ_CSR,							//カーソル
	ROULETTE_ID_OBJ_ITEMKEEP,						//アイテム持たせているアイコン
	//ROULETTE_ID_OBJ_ITEMKEEP1,					//アイテム持たせているアイコン1
	//ROULETTE_ID_OBJ_ITEMKEEP2,					//アイテム持たせているアイコン2
	//ROULETTE_ID_OBJ_ITEMKEEP3,					//アイテム持たせているアイコン3
	//ROULETTE_ID_OBJ_ITEMKEEP4,					//アイテム持たせているアイコン4
	ROULETTE_ID_OBJ_ICON1,							//アイコン1
	ROULETTE_ID_OBJ_ICON2,							//アイコン2
	ROULETTE_ID_OBJ_ICON3,							//アイコン3
	ROULETTE_ID_OBJ_ICON4,							//アイコン4
	ROULETTE_ID_OBJ_ICON5,							//アイコン5
	ROULETTE_ID_OBJ_ICON6,							//アイコン6
	ROULETTE_ID_OBJ_ICON7,							//アイコン7
	ROULETTE_ID_OBJ_ICON8,							//アイコン8
	ROULETTE_ID_OBJ_MAX,
};

#define ROULETTE_POKE_ICON_MAX		(8)				//ポケモンアイコン最大数
#define ROULETTE_ITEM_KEEP_MAX		(8)				//アイテム持っているアイコン最大数

//clactに設定する最大アクター数
//パネル16、カーソル1、ポケモンアイコン8、アイテム持っているアイコン8
//パネル16、カーソル1、ポケモンアイコン8、アイテム持っているアイコン8、ボタン1
//#define ROULETTE_CLACT_OBJ_MAX	(ROULETTE_PANEL_MAX + 1 + ROULETTE_POKE_ICON_MAX + ROULETTE_ITEM_KEEP_MAX)
#define ROULETTE_CLACT_OBJ_MAX	(ROULETTE_PANEL_MAX + 1 + ROULETTE_POKE_ICON_MAX + ROULETTE_ITEM_KEEP_MAX + 1)

//アニメナンバー(カーソル、パネルリソース)
enum{
	ROULETTE_ANM_EV_MINE_HP_DOWN = 0,
	ROULETTE_ANM_EV_MINE_DOKU,
	ROULETTE_ANM_EV_MINE_MAHI,
	ROULETTE_ANM_EV_MINE_YAKEDO,
	ROULETTE_ANM_EV_MINE_NEMURI,
	ROULETTE_ANM_EV_MINE_KOORI,
	ROULETTE_ANM_EV_MINE_SEED_GET,
	ROULETTE_ANM_EV_MINE_ITEM_GET,
	ROULETTE_ANM_EV_MINE_LV_UP,
	////////////////////////////////////////////////
	ROULETTE_ANM_EV_ENEMY_HP_DOWN,
	ROULETTE_ANM_EV_ENEMY_DOKU,					//10
	ROULETTE_ANM_EV_ENEMY_MAHI,
	ROULETTE_ANM_EV_ENEMY_YAKEDO,
	ROULETTE_ANM_EV_ENEMY_NEMURI,
	ROULETTE_ANM_EV_ENEMY_KOORI,
	ROULETTE_ANM_EV_ENEMY_SEED_GET,
	ROULETTE_ANM_EV_ENEMY_ITEM_GET,
	ROULETTE_ANM_EV_ENEMY_LV_UP,
	////////////////////////////////////////////////
	ROULETTE_ANM_EV_PLACE_HARE,
	ROULETTE_ANM_EV_PLACE_AME,
	ROULETTE_ANM_EV_PLACE_SUNAARASI,			//20
	ROULETTE_ANM_EV_PLACE_ARARE,
	ROULETTE_ANM_EV_PLACE_KIRI,
	ROULETTE_ANM_EV_PLACE_TRICK,
	ROULETTE_ANM_EV_PLACE_SPEED_UP,
	ROULETTE_ANM_EV_PLACE_SPEED_DOWN,			//25
	ROULETTE_ANM_EV_PLACE_RANDOM,
	////////////////////////////////////////////////
	ROULETTE_ANM_EV_EX_POKE_CHANGE,				//27
	ROULETTE_ANM_EV_EX_BP_GET,					//28
	ROULETTE_ANM_EV_EX_BTL_WIN,					//29
	ROULETTE_ANM_EV_EX_SUKA,					//30
	ROULETTE_ANM_EV_EX_BP_GET_BIG,				//31
	ROULETTE_ANM_COUNT_3,
	ROULETTE_ANM_COUNT_2,
	ROULETTE_ANM_COUNT_1,
	ROULETTE_ANM_CSR,
};

//アニメナンバー(ボタンリソース)
enum{
	ROULETTE_ANM_BUTTON_STOP = 0,
	ROULETTE_ANM_BUTTON_PUSH,
	ROULETTE_ANM_BUTTON_START,
};

#if 0
//セルナンバー
enum{
	ROULETTE_CELL_CURSOR_L = 0,
	ROULETTE_CELL_CURSOR_R, 
	ROULETTE_CELL_CURSOR_ITEM, 
	ROULETTE_CELL_SCR_CURSOR_U, 
	ROULETTE_CELL_SCR_CURSOR_D, 
	ROULETTE_CELL_RANK_CURSOR_L,
	ROULETTE_CELL_RANK_CURSOR_R, 
};
#endif

#define BR_FRAME_WIN	(GF_BGL_FRAME1_M)
#define BR_FRAME_TYPE	(GF_BGL_FRAME0_M)
#define BR_FRAME_EFF	(GF_BGL_FRAME2_M)			//パネルエフェクト
#define BR_FRAME_BG		(GF_BGL_FRAME3_M)			//SINGLE,DOUBLE
#define BR_FRAME_SUB	(GF_BGL_FRAME0_S)			//ボール

typedef struct{
	s16  x;
	s16  y;
}ROULETTE_POS;


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern const PROC_DATA RouletteProcData;				//プロセス定義データ
extern PROC_RESULT RouletteProc_Init( PROC * proc, int * seq );
extern PROC_RESULT RouletteProc_Main( PROC * proc, int * seq );
extern PROC_RESULT RouletteProc_End( PROC * proc, int * seq );

//通信
extern void Roulette_CommRecvBufBasicData(int id_no,int size,void *pData,void *work);
extern void Roulette_CommRecvBufRankUpType(int id_no,int size,void *pData,void *work);
extern void Roulette_CommRecvBufCsrPos(int id_no,int size,void *pData,void *work);
extern void Roulette_CommRecvBufModoru(int id_no,int size,void *pData,void *work);


#endif //_ROULETTE_SYS_H_


