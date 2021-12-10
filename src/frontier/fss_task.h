//==============================================================================
/**
 * @file	fss_task.h
 * @brief	フロンティア用スクリプト仮想マシンのヘッダ
 * @author	matsuda
 * @date	2007.03.29(木)
 */
//==============================================================================
#ifndef __FSS_TASK_H__
#define __FSS_TASK_H__

#include "gflib/msg_print.h"
#include "system/msgdata.h"

#include "fs_usescript.h"


//==============================================================================
//	定数定義
//==============================================================================
///スクリプト制御タスク最大数
#define FSS_TASK_MAX		(8)

#define FSST_STACK_MAX	( 20 )	// 仮想マシンのスタックの深さ


//==============================================================================
//	型定義
//==============================================================================
typedef struct _FS_SYSTEM FS_SYSTEM;

typedef	u8	FSS_CODE;				// スクリプト、イベントデータの型宣言

typedef struct _FSS_TASK FSS_TASK;
typedef BOOL ( * FRSCR_CMD)(FSS_TASK *);

typedef BOOL ( * FSST_WAIT_FUNC)(FSS_TASK *);

//==============================================================================
//	構造体定義
//==============================================================================
///スクリプト制御タスク構造体
typedef struct _FSS_TASK{
	FS_SYSTEM *fss;	///<スクリプト制御ワークへのポインタ
	
	u32 seqno;
	u16 local_work[FSW_LOCAL_MAX - FSW_LOCAL_START];

	u8	SP;									// スタックポインタ
	u8	status;								// 動作状態
	const FSS_CODE * PC;						// 現在のスクリプトアドレス
	const FSS_CODE * array[FSST_STACK_MAX];	// スタック
	const FRSCR_CMD * command_table;			// スクリプトコマンドテーブル開始位置
	u32 cmd_max;							// コマンド上限値
	u16 reg[FSW_REG_MAX - FSW_REG_START];	// 汎用レジスタ

	MSGDATA_MANAGER* msgman;				// メッセージマネージャー
	FSS_CODE * pScript;						// ロードしたスクリプト
	FSST_WAIT_FUNC routine;					// ウェイト関数

	u8	cmp_flag;							// 計算結果フラグ

//	void * event_work;						// コマンドなどで参照するワークのポインタ

}FSS_TASK;


// u8サイズのデータを取得するためのマクロ
#define FSSTGetU8(core)	(*((core)->PC++))


//==============================================================================
//	外部データ宣言
//==============================================================================
extern const FRSCR_CMD FSSCmdTable[];
extern const u32 FSSCmdTableMax;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern void FSST_Init( FSS_TASK * core, const FRSCR_CMD * cmd_tbl, u32 cmd_max);
extern u8 FSST_Start( FSS_TASK * core, const FSS_CODE * start );
extern void FSST_SetWait( FSS_TASK * core, FSST_WAIT_FUNC func );
extern void FSST_End( FSS_TASK * core );
extern void FSST_SetWork( FSS_TASK * core, void * work );
extern u8 FSST_Control( FSS_TASK * core );
extern u8 FSSTStackPush( FSS_TASK * core, const FSS_CODE * val );
extern const FSS_CODE * FSSTStackPop( FSS_TASK * core );
extern void FSSTJump( FSS_TASK * core, FSS_CODE * adrs );
extern void FSSTCall( FSS_TASK * core, FSS_CODE * adrs );
extern void FSSTRet( FSS_TASK * core );
extern u16 FSSTGetU16( FSS_TASK * core );
extern u32 FSSTGetU32( FSS_TASK * core );


#endif	//__FSS_TASK_H__

