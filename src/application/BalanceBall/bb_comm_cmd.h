//==============================================================================
/**
 * @file	bb_comm_cmd.h
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.09.25(火)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#ifndef __BB_COMM_CMD_H__
#define __BB_COMM_CMD_H__

#include "communication/communication.h"

#include "bb_common.h"

enum CommCommandBB {
	
	CCMD_BB_START	= CS_COMMAND_MAX,	///< 通信開始
	CCMD_BB_END,						///< 親 > 子
	CCMD_BB_STATE,						///< 子 > 全
	CCMD_BB_RESULT,						///< 子 > 全
	CCMD_BB_SCORE,						///< 親 > 全
	
	CCMD_BB_CONNECT_END,	
};

extern const CommPacketTbl* BB_CommCommandTclGet( void );
extern int BB_CommCommandTblNumGet( void );
extern void  BB_CommCommandInit( BB_WORK* wk );

#endif	//