
//============================================================================
/**
 *
 *@file		sub_023.s
 *@brief	戦闘シーケンス
 *			どくによるダメージシーケンス
 *@author	HisashiSogabe
 *@data		2005.12.02
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_023:
	//特性マジックガードは、ダメージなし
	TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_WORK,TOKUSYU_MAZIKKUGAADO,SUB_023_END
	//特性ポイズンヒールは、HPを回復
	TOKUSEI_CHECK	TOKUSEI_NO_HAVE,SIDE_WORK,TOKUSYU_POIZUNHIIRU,SUB_023_NEXT
	PSP_VALUE_WORK	VAL_GET,SIDE_WORK,ID_PSP_hpmax,BUF_PARA_HP_CALC_WORK
	IF_PSP_WORK		IF_FLAG_EQ,SIDE_WORK,ID_PSP_hp,BUF_PARA_HP_CALC_WORK,SUB_023_END
	DAMAGE_DIV		BUF_PARA_HP_CALC_WORK,8 
	VALUE			VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_NO_BLINK
	GOSUB			SUB_SEQ_HP_CALC
	MESSAGE			TokuseiHPKaifukuMineMsg,TAG_NICK_TOKU,SIDE_WORK,SIDE_CLIENT_WORK
	SERVER_WAIT
	WAIT			MSG_WAIT
	SEQ_END
SUB_023_NEXT:
	MESSAGE			DokuDamageMineMsg,TAG_NICK,SIDE_WORK
	SERVER_WAIT
	WAIT			MSG_WAIT
	STATUS_EFFECT	SIDE_WORK,STATUS_DOKU
	SERVER_WAIT
	VALUE			VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_NO_BLINK
	JUMP			SUB_SEQ_HP_CALC
SUB_023_END:
	SEQ_END
