//==============================================================================================
/**
 * @file	castle_sys.h
 * @brief	「バトルキャッスル」システムヘッダー
 * @author	Satoshi Nohara
 * @date	07.07.05
 */
//==============================================================================================
#ifndef _CASTLE_SYS_H_
#define _CASTLE_SYS_H_

#include "common.h"
#include "system/procsys.h"


//==============================================================================================
//
//	タクトワークへの不完全型ポインタ
//
//==============================================================================================
typedef	struct _CASTLE_MINE_WORK CASTLE_MINE_WORK;
typedef	struct _CASTLE_ENEMY_WORK CASTLE_ENEMY_WORK;
typedef	struct _CASTLE_RANK_WORK CASTLE_RANK_WORK;


//==============================================================================================
//
//	定義
//
//==============================================================================================
//パレット定義
#define BC_TALKWIN_PAL			(10)				//会話ウィンドウ使用パレットナンバー
#define BC_MENU_PAL				(11)				//メニューウィンドウ使用パレットナンバー
#define BC_MSGFONT_PAL			(12)				//MSGフォント使用パレットナンバー
#define BC_FONT_PAL				(13)				//SYSTEMフォント使用パレットナンバー

#define CASTLE_VANISH_ON		(0)					//非表示
#define CASTLE_VANISH_OFF		(1)					//表示

//リソースマネージャーオブジェクトナンバー
enum{
	RES_OBJ_CSR = 0,								//カーソル
	RES_OBJ_ITEMICON,								//アイテムアイコン
	RES_OBJ_ITEMKEEP,								//アイテム持たせているアイコン
	RES_OBJ_ICON1,									//アイコン1
	RES_OBJ_ICON2,									//アイコン2
	RES_OBJ_ICON3,									//アイコン3
	RES_OBJ_ICON4,									//アイコン4
	RES_OBJ_MAX,									//最大数(リソースファイルは1つ、あとはアイコン)
};

//管理CHAR_ID
enum{
	ID_CHAR_CSR = 0,								//カーソル
	ID_CHAR_ITEMICON,								//アイテムアイコン
	ID_CHAR_ITEMKEEP,								//アイテム持たせているアイコン
	ID_CHAR_ICON1,									//アイコン1
	ID_CHAR_ICON2,									//アイコン2
	ID_CHAR_ICON3,									//アイコン3
	ID_CHAR_ICON4,									//アイコン4
	ID_CHAR_MAX,									//
};

//管理PLTT_ID
enum{
	ID_PLTT_CSR = 0,								//カーソル
	ID_PLTT_ITEMICON,								//アイテムアイコン
	ID_PLTT_ITEMKEEP,								//アイテム持たせているアイコン
	ID_PLTT_ICON,									//アイコン
	ID_PLTT_MAX,									//
};

//管理CELL_ID
enum{
	ID_CELL_CSR = 0,								//カーソル
	ID_CELL_ITEMICON,								//アイテムアイコン
	ID_CELL_ITEMKEEP,								//アイテム持たせているアイコン
	ID_CELL_ICON,									//アイコン
	ID_CELL_MAX,									//
};

//管理CELLANM_ID
enum{
	ID_CELLANM_CSR = 0,								//カーソル
	ID_CELLANM_ITEMICON,							//アイテムアイコン
	ID_CELLANM_ITEMKEEP,							//アイテム持たせているアイコン
	ID_CELLANM_ICON,								//アイコン
	ID_CELLANM_MAX,									//
};

//管理ID
enum{
	ID_OBJ_CSR = 0,									//カーソル
	ID_OBJ_ITEMICON,								//アイテムアイコン
	ID_OBJ_ITEMKEEP,								//アイテム持たせているアイコン
	//ID_OBJ_ITEMKEEP1,								//アイテム持たせているアイコン1
	//ID_OBJ_ITEMKEEP2,								//アイテム持たせているアイコン2
	//ID_OBJ_ITEMKEEP3,								//アイテム持たせているアイコン3
	//ID_OBJ_ITEMKEEP4,								//アイテム持たせているアイコン4
	ID_OBJ_ICON1,									//アイコン1
	ID_OBJ_ICON2,									//アイコン2
	ID_OBJ_ICON3,									//アイコン3
	ID_OBJ_ICON4,									//アイコン4
	ID_OBJ_MAX,
};

//MINE:POKEICON 4 + ITEMKEEP 4 + CSR 2 + SCR 2 + POKESEL 1 + EFF 1 + HP 4 + ITEMICON 1  =
//ENEMY:POKEICON 4 + ITEMKEEP 4 + CSR 2 + SCR 2 + POKESEL 1 + EFF 1 + HP 4				= 
//RANK:CSR 2 + POKESEL 1 + EFF 1 + HATA 2*(3*3)											= 22

//アイコン4 + アイテム持っているアイコン4 + カーソル2 + スクロールカーソル2
//ポケ選択カーソル1 + エフェクト1 + HPバー4 + 旗2*(3*3)
//#define CLACT_OBJ_MAX	(12)//(8)							//セルアクターに設定する最大アクター数
#define CLACT_OBJ_MAX	(40)//(8)							//セルアクターに設定する最大アクター数

//アニメナンバー
enum{
	CASTLE_ANM_MODORU_ZIBUN = 0,
	CASTLE_ANM_SELECT_ZIBUN,
	CASTLE_ANM_SELECT_AITE,
	CASTLE_ANM_CURSOR_ITEM, 
	CASTLE_ANM_SCR_CURSOR_U, 
	CASTLE_ANM_SCR_CURSOR_D, 
	CASTLE_ANM_BALL_STOP, 
	CASTLE_ANM_BALL_MOVE, 
	CASTLE_ANM_KAIHUKU_EFF, 
	CASTLE_ANM_LV_UP, 
	CASTLE_ANM_LV_DOWN, 
	CASTLE_ANM_RANK_UP, 
	CASTLE_ANM_HATA, 
	CASTLE_ANM_HP_AKA, 
	CASTLE_ANM_HP_KIIRO, 
	CASTLE_ANM_HP_MIDORI, 
	CASTLE_ANM_ITEMGET_EFF, 
	CASTLE_ANM_MODORU_AITE, 
};

#define BC_FRAME_WIN	(GF_BGL_FRAME1_M)
#define BC_FRAME_TYPE	(GF_BGL_FRAME0_M)
#define BC_FRAME_SLIDE	(GF_BGL_FRAME2_M)			//MULTIとレンタル決定時
#define BC_FRAME_BG		(GF_BGL_FRAME3_M)			//SINGLE,DOUBLE
#define BC_FRAME_SUB	(GF_BGL_FRAME0_S)			//ボール

typedef struct{
	s16  x;
	s16  y;
}POS;

//決定無効の定義
#define CASTLE_DECIDE_TYPE_NONE	(0xff)

#define CASTLE_RANK_MAX			(3)					//最大ランク3

//#define CASTLE_COMM_WAIT		(60)				//通信ウェイト
#define CASTLE_COMM_WAIT		(30)				//通信ウェイト


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern const PROC_DATA CastleProcData;				//プロセス定義データ
extern PROC_RESULT CastleProc_Init( PROC * proc, int * seq );
extern PROC_RESULT CastleProc_Main( PROC * proc, int * seq );
extern PROC_RESULT CastleProc_End( PROC * proc, int * seq );

//通信
extern void CastleMine_CommRecvBufBasicData(int id_no,int size,void *pData,void *work);
extern void CastleMine_CommRecvBufRankUpType(int id_no,int size,void *pData,void *work);
extern void CastleMine_CommRecvBufCsrPos(int id_no,int size,void *pData,void *work);
extern void CastleMine_CommRecvBufModoru(int id_no,int size,void *pData,void *work);
extern void CastleEnemy_CommRecvBufBasicData(int id_no,int size,void *pData,void *work);
extern void CastleEnemy_CommRecvBufRankUpType(int id_no,int size,void *pData,void *work);
extern void CastleEnemy_CommRecvBufCsrPos(int id_no,int size,void *pData,void *work);
extern void CastleEnemy_CommRecvBufModoru(int id_no,int size,void *pData,void *work);
extern void CastleRank_CommRecvBufBasicData(int id_no,int size,void *pData,void *work);
extern void CastleRank_CommRecvBufRankUpType(int id_no,int size,void *pData,void *work);
extern void CastleRank_CommRecvBufCsrPos(int id_no,int size,void *pData,void *work);
extern void CastleRank_CommRecvBufModoru(int id_no,int size,void *pData,void *work);


#endif //_CASTLE_SYS_H_


