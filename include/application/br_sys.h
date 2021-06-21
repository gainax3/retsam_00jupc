//==============================================================================
/**
 * @file	br_sys.h
 * @brief	バトルレコーダー
 * @author	goto
 * @date	2007.07.26(木)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#ifndef	__BR_SYS_H__
#define __BR_SYS_H__

#include "include/application/br_sys_def.h"


// -------------------------------------
//
//	プロセスデータ
//
// -------------------------------------
extern const PROC_DATA	BattleRecorder_ManagerProcData;		///< 管理

extern const PROC_DATA	BattleRecorder_BrowseProcData;
extern const PROC_DATA	BattleRecorder_GDSProcData;

extern const PROC_DATA* BattleRecoder_ProcDataGet( int mode );



#endif	//__BR_SYS_H__