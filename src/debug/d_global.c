//==============================================================================
/**
 * @file	d_global.c
 * @brief	デバッグで使用したいグローバル変数などの定義場所
 * @author	matsuda
 * @date	2008.02.20(水)
 */
//==============================================================================
#ifdef PM_DEBUG

int d_test = 0;
#ifdef DEBUG_ONLY_FOR_nishino
u32		DebugBattleFlag=0;		///<デバッグ戦闘用フラグワーク
#endif

#endif	//PM_DEBUG
