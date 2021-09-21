//==============================================================================
/**
 * @file	battle_cursor_disp.c
 * @brief	戦闘の初期カーソル表示状態記憶用構造体
 * @author	matsuda
 * @date	2007.08.09(木)
 */
//==============================================================================
#include "common.h"
#include "battle/battle_cursor_disp.h"


//--------------------------------------------------------------
/**
 * @brief   戦闘カーソル初期表示状態ワークの確保
 *
 * @param   heap_id		ヒープID
 *
 * @retval  戦闘カーソル初期表示状態ワークへのポインタ
 */
//--------------------------------------------------------------
BATTLE_CURSOR_DISP * BattleCursorDisp_AllocWork(int heap_id)
{
	BATTLE_CURSOR_DISP *bcd;
	
	bcd = sys_AllocMemory(heap_id, sizeof(BATTLE_CURSOR_DISP));
	MI_CpuClear8(bcd, sizeof(BATTLE_CURSOR_DISP));
	return bcd;
}

//--------------------------------------------------------------
/**
 * @brief   戦闘カーソル初期表示状態ワークの解放
 *
 * @param   bcd		戦闘カーソル初期表示状態ワークへのポインタ
 */
//--------------------------------------------------------------
void BattleCursorDisp_Free(BATTLE_CURSOR_DISP *bcd)
{
	sys_FreeMemoryEz(bcd);
}

