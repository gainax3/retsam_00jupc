//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		comm_bct_command.h
 *	@brief		バケットミニゲーム　通信コマンド
 *	@author		tomoya takahashi
 *	@data		2007.06.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __COMM_BCT_COMMAND_H__
#define __COMM_BCT_COMMAND_H__
#include "communication/communication.h"

enum CommCommandBCT {

	//-------------------------------------
	//　ゲーム用
	//=====================================
	CNM_BCT_START = CS_COMMAND_MAX,	//< 通信開始
	CNM_BCT_END,					///< ゲーム終了				親ー＞子
	CNM_BCT_NUTS,					///< 木の実データ			子ー＞その他みんな
	CNM_BCT_SCORE,					///< 自分の得点				子ー＞親
	CNM_BCT_ALLSCORE,				///< 全員の得点				親ー＞子　結果発表のタイミングにも使用
	CNM_BCT_GAME_LEVEL,				///< ゲームレベルの送信		親ー＞子　
	CNM_BCT_MIDDLESCORE,			///< 途中の自分の得点		子ー＞子
	CNM_BCT_MIDDLESCORE_OK,			///< 全員の得点がきた		親ー＞子
	

	//-------------------------------------
	//	
	//=====================================
	
	//------------------------------------------------ここまで
	CNM_COMMAND_MAX   // 終端--------------これは移動させないでください
};

extern const CommPacketTbl* BCT_CommCommandTclGet( void );
extern int BCT_CommCommandTblNumGet( void );

#endif		// __COMM_BCT_COMMAND_H__

