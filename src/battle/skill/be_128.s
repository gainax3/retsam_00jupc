//============================================================================
/**
 *
 *@file		be_128.s
 *@brief	戦闘シーケンス（BattleEffect）
 *			128　敵がポケモンを引っ込める時にこの技を選んでいると、交代する前に倍のダメージを与えることが出来る
 *
 *@author	HisashiSogabe
 *@data		2006.02.09
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

BE_128:
	CRITICAL_CHECK
	DAMAGE_CALC
	SEQ_END
