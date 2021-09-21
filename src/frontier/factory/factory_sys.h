//==============================================================================================
/**
 * @file	factory_sys.h
 * @brief	「バトルファクトリー」システムヘッダー
 * @author	Satoshi Nohara
 * @date	2007.03.15
 */
//==============================================================================================
#ifndef _FACTORY_SYS_H_
#define _FACTORY_SYS_H_

#include "common.h"
#include "system/procsys.h"


//#define DEBUG_ON									//デバック処理有効


//==============================================================================================
//
//	タクトワークへの不完全型ポインタ
//
//==============================================================================================
typedef	struct _FACTORY_WORK FACTORY_WORK;


//==============================================================================================
//
//	定義
//
//==============================================================================================
//パレット定義
#define BF_TALKWIN_PAL			(10)				//会話ウィンドウ使用パレットナンバー
#define BF_MENU_PAL				(11)				//メニューウィンドウ使用パレットナンバー
#define BF_MSGFONT_PAL			(12)				//MSGフォント使用パレットナンバー
#define BF_FONT_PAL				(13)				//SYSTEMフォント使用パレットナンバー

#define FACTORY_VANISH_ON		(0)					//非表示
#define FACTORY_VANISH_OFF		(1)					//表示

//リソースマネージャーオブジェクトナンバー
enum{
	FACTORY_RES_OBJ_BALL = 0,						//ボール、カーソルなど
	FACTORY_RES_OBJ_MAX,							//最大数(リソースファイルは1つのみ)
};

//管理ID
enum{
	FACTORY_ID_OBJ_BALL	= 0,						//ボール
};

//管理出来る動物OBJの最大数
#define BALL_OBJ_MAX			(6)					//セルアクターに設定するアクター最大数
#define CSR_OBJ_MAX				(2)					//セルアクターに設定するアクター最大数
#define WIN_OBJ_MAX				(1)
#define FACTORY_CLACT_OBJ_MAX	(BALL_OBJ_MAX+CSR_OBJ_MAX+WIN_OBJ_MAX)		//今は適当

//アニメナンバー

enum{
	ANM_SINGLE_OPEN	= 0,
	ANM_SINGLE_CLOSE,
	ANM_MULTI_OPEN,
	ANM_MULTI_CLOSE,
	ANM_3POKE_OPEN,
	ANM_3POKE_CLOSE,
	ANM_BALL_STOP,						//ボール停止
	ANM_BALL_MOVE,						//ボール動く
	ANM_BALL_CSR,						//ボールカーソル
	ANM_MENU_CSR,						//メニューカーソル
	ANM_BALL_YURE,						//ボールゆれ
	ANM_MULTI_2POKE_OPEN,
	ANM_MULTI_2POKE_CLOSE,
	ANM_BALL_CSR_STOP,					//ボールカーソル停止
};

#define BF_FRAME_WIN	(GF_BGL_FRAME1_M)
#define BF_FRAME_SLIDE	(GF_BGL_FRAME2_M)			//MULTIとレンタル決定時
#define BF_FRAME_BG		(GF_BGL_FRAME3_M)			//SINGLE,DOUBLE
#define BF_FRAME_SUB	(GF_BGL_FRAME0_S)			//ボール

typedef struct{
	s16  x;
	s16  y;
}FACTORY_POS;


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern const PROC_DATA FactoryProcData;				//プロセス定義データ
extern PROC_RESULT FactoryProc_Init( PROC * proc, int * seq );
extern PROC_RESULT FactoryProc_Main( PROC * proc, int * seq );
extern PROC_RESULT FactoryProc_End( PROC * proc, int * seq );

//通信
extern void Factory_CommRecvBufBasicData(int id_no,int size,void *pData,void *work);
extern void Factory_CommRecvBufRentalData(int id_no,int size,void *pData,void *work);
extern void Factory_CommRecvBufCancelData(int id_no,int size,void *pData,void *work);
extern void Factory_CommRecvBufTradeFlag(int id_no,int size,void *pData,void *work);


#endif //_FACTORY_SYS_H_


