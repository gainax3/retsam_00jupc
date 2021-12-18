//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		comm_command_wfp2pmf.h
 *	@brief		２～４人専用待合室	通信コマンド
 *	@author		tomoya takahashi
 *	@data		2007.05.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __COMM_COMMAND_WFP2PMF_H__
#define __COMM_COMMAND_WFP2PMF_H__



#undef GLOBAL
#ifdef	__COMM_COMMAND_WFP2PMF_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

enum CommCommandTradeList_e {

	//-------------------------------------
	//　ゲーム用
	//=====================================
	// 同期用	
	CNM_WFP2PMF_RESULT = CS_COMMAND_MAX,	///< 通信KONG		親ー＞子
	CNM_WFP2PMF_START,						///< ゲーム開始		親ー＞子
	CNM_WFP2PMF_VCHAT,						///< VCHATデータ	親ー＞子

	//-------------------------------------
	//	
	//=====================================
	
	//------------------------------------------------ここまで
	CNM_COMMAND_MAX   // 終端--------------これは移動させないでください
};

GLOBAL const CommPacketTbl* WFP2PMF_CommCommandTclGet( void );
GLOBAL int WFP2PMF_CommCommandTblNumGet( void );

#undef	GLOBAL
#endif		// __COMM_COMMAND_WFP2PMF_H__

