//============================================================================
/**
 *
 *@file		be_203.s
 *@brief	戦闘シーケンス（BattleEffect）
 *			203　天気によってタイプが変わる　はれ：炎　あめ：水　すなあらし：岩　あられ：氷　天気状態の時、威力が倍になる
 *
 *@author	HisashiSogabe
 *@data		2006.02.09
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

BE_003:
	WEATHER_BALL
	CRITICAL_CHECK
	DAMAGE_CALC
	SEQ_END
