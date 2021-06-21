
//============================================================================
/**
 *
 *@file		sub_294.s
 *@brief	戦闘シーケンス
 *			フィールド天候で晴れ
 *@author	HisashiSogabe
 *@data		2007.09.11
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_294:
	STATUS_EFFECT	SIDE_MINE,STATUS_WEATHER_SUN
	SERVER_WAIT
	MESSAGE			NihonbareMsg,TAG_NONE
	SERVER_WAIT
	WAIT			MSG_WAIT
	VALUE			VAL_BIT,BUF_PARA_FIELD_CONDITION,FIELD_CONDITION_HIDERI
	SEQ_END
