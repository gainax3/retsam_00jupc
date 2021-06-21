//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wbr_comm_command.h
 *	@brief
 *	@author	
 *	@data		2007.02.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WBR_COMM_COMMAND_H__
#define __WBR_COMM_COMMAND_H__

#include "communication/comm_command.h"

#undef GLOBAL
#ifdef	__WBR_COMM_COMMAND_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
enum WbrCommCommand {

	//-------------------------------------
	//　ゲーム用
	//=====================================
	// 同期用	
	CNM_WBR_GAME_SYS_START = CS_COMMAND_MAX,	///< OYA->KO 準備完了
	CNM_WBR_GAME_SYS_GAMESTART,					///< OYA->KO 通信処理スタート
	CNM_WBR_GAME_SYS_END,						///< OYA->KO 終了 
	
	// データ受信用
	CNM_WBR_KO_COMMON,		///< KO->OYA 共通データ
	CNM_WBR_KO_GAMEDATA,	///< KO->OYA ゲームコマンド
	CNM_WBR_OYA_GAMEDATA,	///< OYA->KO ゲーム状態データ
	CNM_WBR_KO_OYADATA_GET,	///< KO->OYA 現在のゲーム状態強制取得
	CNM_WBR_KO_TALKDATA,	///< KO->OYA 話しかけデータ
	CNM_WBR_KO_TALKREQ,		///< KO->OYA 話しかけリクエスト

	//-------------------------------------
	//	
	//=====================================
	
	//------------------------------------------------ここまで
	CNM_COMMAND_MAX   // 終端--------------これは移動させないでください
};

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
GLOBAL const CommPacketTbl* Wbr_CommCommandTclGet( void );

GLOBAL int Wbr_CommCommandTblNumGet( void );

#undef	GLOBAL
#endif		// __WBR_COMM_COMMAND_H__

