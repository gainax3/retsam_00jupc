//==============================================================================================
/**
 * @file	stage_sys.h
 * @brief	「バトルステージ」システムヘッダー
 * @author	Satoshi Nohara
 * @date	07.06.08
 */
//==============================================================================================
#ifndef _STAGE_SYS_H_
#define _STAGE_SYS_H_

#include "common.h"
#include "system/procsys.h"


//==============================================================================================
//
//	タクトワークへの不完全型ポインタ
//
//==============================================================================================
typedef	struct _STAGE_WORK STAGE_WORK;


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

#define STAGE_VANISH_ON			(0)					//非表示
#define STAGE_VANISH_OFF		(1)					//表示

//リソースマネージャーオブジェクトナンバー
enum{
	STAGE_RES_OBJ_CSR = 0,								//カーソル
	STAGE_RES_OBJ_ICON,									//アイコン
	STAGE_RES_OBJ_MAX,									//最大数(リソースファイルは1つ、あとはアイコン)
};

//管理ID
enum{
	STAGE_ID_OBJ_CSR = 0,								//カーソル
	STAGE_ID_OBJ_ICON,									//アイコン
	STAGE_ID_OBJ_MAX,
};

#define STAGE_CLACT_OBJ_MAX	(2)						//セルアクターに設定する最大アクター数

//アニメナンバー
enum{
	ANM_CSR_STOP = 0,								//カーソル停止中
	ANM_CSR_MOVE,									//カーソル移動中
	ANM_CSR_POKE,									//カーソルポケモン
};

#define BS_FRAME_WIN	(GF_BGL_FRAME0_M)
#define BS_FRAME_TYPE	(GF_BGL_FRAME1_M)
#define BS_FRAME_SLIDE	(GF_BGL_FRAME2_M)			//MULTIとレンタル決定時
#define BS_FRAME_BG		(GF_BGL_FRAME3_M)			//SINGLE,DOUBLE
#define BS_FRAME_SUB	(GF_BGL_FRAME0_S)			//ボール

typedef struct{
	s16  x;
	s16  y;
}STAGE_POS;


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern const PROC_DATA StageProcData;				//プロセス定義データ
extern PROC_RESULT StageProc_Init( PROC * proc, int * seq );
extern PROC_RESULT StageProc_Main( PROC * proc, int * seq );
extern PROC_RESULT StageProc_End( PROC * proc, int * seq );

//通信
extern void Stage_CommSendBufBasicData( STAGE_WORK* wk, u16 type );
extern void Stage_CommRecvBufBasicData(int id_no,int size,void *pData,void *work);
extern void Stage_CommSendBufPokeTypeData( STAGE_WORK* wk, u16 type, u16 param );
extern void Stage_CommRecvBufPokeTypeData(int id_no,int size,void *pData,void *work);
extern void Stage_CommSendBufFinalAnswerData( STAGE_WORK* wk, u16 type, u16 param );
extern void Stage_CommRecvBufFinalAnswerData(int id_no,int size,void *pData,void *work);


#endif //_STAGE_SYS_H_


